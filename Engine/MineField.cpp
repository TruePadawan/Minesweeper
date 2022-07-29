#include "MineField.h"
#include <random>
#include <assert.h>
#include <algorithm>

MineField::MineField(int _nMines)
{
    nMines = _nMines;
    isMineTriggered = false;
    for (int y = 0; y < TILES_PER_HEIGHT; ++y)
    {
        for (int x = 0; x < TILES_PER_WIDTH; ++x)
        {
            tileAt({x, y}) = Tile({x, y});
        }
    }

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> xDist(0, TILES_PER_WIDTH - 1);
    std::uniform_int_distribution<int> yDist(0, TILES_PER_HEIGHT - 1);

	for (int i = 0; i < nMines; ++i)
	{
		Vei2 gridPos = { 0,0 };
		do {
			gridPos = { xDist(rng), yDist(rng) };
		} while (tileAt(gridPos).hasMine);
		tileAt(gridPos).spawnMine();
	}

    for (int y = 0; y < TILES_PER_HEIGHT; ++y)
    {
        for (int x = 0; x < TILES_PER_WIDTH; ++x)
        {
            Tile& tile{ tileAt({ x, y }) };
            tile.setNumberOfAdjacentMines(getNumberOfAdjacentMines(tile));
        }
    }
}

MineField::Tile::Tile(const Vei2& pos)
    :gridPos(pos), state(State::Hidden), hasMine(false)
{
}

void MineField::Tile::draw(Graphics& gfx, bool mineTriggered)
{
    Vei2 pixelPos = gridToPixelPosition(gridPos);
    if (mineTriggered)
    {
        switch (state)
        {
        case MineField::Tile::State::Revealed:
        {
            if (hasMine)
            {
                SpriteCodex::DrawTileBombRed(pixelPos, gfx);
            }
            else {
                SpriteCodex::DrawTileNumber(pixelPos, nAdjacentMines, gfx);
            }
        }
        break;
        case MineField::Tile::State::Flagged:
            if (hasMine)
            {
                SpriteCodex::DrawTileButton(pixelPos, gfx);
                SpriteCodex::DrawTileFlag(pixelPos, gfx);
            }
            else {
                SpriteCodex::DrawTileBomb(pixelPos, gfx);
                SpriteCodex::DrawTileCross(pixelPos, gfx);
            }
            break;
        case MineField::Tile::State::Hidden:
            if (hasMine)
            {
                SpriteCodex::DrawTileBomb(pixelPos, gfx);
            }
            else {
                SpriteCodex::DrawTileButton(pixelPos, gfx);
            }
            break;
        }
    }
    else {
        switch (state)
        {
        case MineField::Tile::State::Revealed:
            {
                if (hasMine)
                {
                    SpriteCodex::DrawTileBomb(pixelPos, gfx);
                }
                else {
                    SpriteCodex::DrawTileNumber(pixelPos, nAdjacentMines, gfx);
                }
            }
            break;
        case MineField::Tile::State::Flagged:
            SpriteCodex::DrawTileButton(pixelPos, gfx);
            SpriteCodex::DrawTileFlag(pixelPos, gfx);
            break;
        case MineField::Tile::State::Hidden:
            SpriteCodex::DrawTileButton(pixelPos, gfx);
            break;
        }
    }
}

void MineField::Tile::spawnMine()
{
    assert(!hasMine);
    hasMine = true;
}

void MineField::Tile::reveal()
{
    if (state == State::Hidden)
    {
        state = State::Revealed;
    }
}

void MineField::Tile::flag()
{
    if (state == State::Hidden)
    {
        state = State::Flagged;
    }
    else if (state == State::Flagged)
    {
        state = State::Hidden;
    }
}

void MineField::Tile::setNumberOfAdjacentMines(int count)
{
    assert(nAdjacentMines == -1);
    nAdjacentMines = count;
}

void MineField::draw(Graphics& gfx)
{
    int nTiles = TILES_PER_HEIGHT * TILES_PER_WIDTH;
    RectI background{ 0, 20 * 16, 0, 16 * 16 };
    gfx.DrawRect(background, Colors::White);

    for (int i = 0; i < nTiles; ++i)
    {
        minefield[i].draw(gfx, isMineTriggered);
    }
}

bool MineField::mouseIsWithinField(const Mouse& mouse)
{
    int tileSize = SpriteCodex::tileSize;
    Vei2 mouseGridPos{ mouse.GetPos() };
    return (mouseGridPos.x >= 0 && mouseGridPos.x < TILES_PER_WIDTH * tileSize &&
           mouseGridPos.y >= 0 && mouseGridPos.y < TILES_PER_HEIGHT * tileSize);
}

bool MineField::mineTriggered()
{
    return isMineTriggered;
}

void MineField::revealTile(const Vei2& pixelPos)
{
    const Vei2 gridPos{ pixelToGridPosition(pixelPos) };
    tileAt(gridPos).reveal();
    if (tileAt(gridPos).hasMine)
    {
        isMineTriggered = true;
    }
}

void MineField::flagTile(const Vei2& pixelPos)
{
    const Vei2 gridPos{ pixelToGridPosition(pixelPos) };
    minefield[gridPos.y * TILES_PER_WIDTH + gridPos.x].flag();
}

Vei2 MineField::pixelToGridPosition(const Vei2& pixelPos) const
{
	int tileSize = SpriteCodex::tileSize;
	assert(pixelPos.x >= 0 && pixelPos.x < TILES_PER_WIDTH * tileSize && pixelPos.y >= 0 && pixelPos.y < TILES_PER_HEIGHT * tileSize);
	return pixelPos / tileSize;
}

int MineField::getNumberOfAdjacentMines(const Tile& tile)
{
    int xStart = std::max(0, tile.gridPos.x - 1);
    int xEnd = std::min(TILES_PER_WIDTH - 1, tile.gridPos.x + 1);
    int yStart = std::max(0, tile.gridPos.y - 1);
    int yEnd = std::min(TILES_PER_HEIGHT - 1, tile.gridPos.y + 1);
    int count = 0;
    for (Vei2 gridPos = {xStart, yStart}; gridPos.y <= yEnd; ++gridPos.y)
    {
        for (gridPos.x = xStart; gridPos.x <= xEnd; ++gridPos.x)
        {
            if (tileAt(gridPos).hasMine)
            {
                ++count;
            }
        }
    }
    return count;
}

Vei2 MineField::Tile::gridToPixelPosition(const Vei2& gridPos) const
{
    return gridPos * SpriteCodex::tileSize;
}

MineField::Tile& MineField::tileAt(const Vei2& gridPos)
{
    return minefield[gridPos.y * TILES_PER_WIDTH + gridPos.x];
}
