#include "MineField.h"
#include <random>
#include <assert.h>
#include <algorithm>

MineField::MineField(int _nMines)
    :nMines(_nMines), isMineTriggered(false), nRevealedSafeTiles(0)
{
    assert(_nMines > 0 && _nMines < (TILES_PER_WIDTH * TILES_PER_HEIGHT));

    int boundaryRight = MARGIN_LEFT + (TILES_PER_WIDTH * SpriteCodex::tileSize);
    int boundaryBottom = MARGIN_TOP + (TILES_PER_HEIGHT * SpriteCodex::tileSize);
    boundary = RectI(Vei2(MARGIN_LEFT, MARGIN_TOP), Vei2(boundaryRight, boundaryBottom));

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
    // ADD THE MARGIN OFFSET TO ALL PIXELS TO BE DRAWN
    Vei2 pixelPos = gridToPixelPosition(gridPos);
    pixelPos += Vei2(MARGIN_LEFT, MARGIN_TOP);
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
                SpriteCodex::DrawTileBomb(pixelPos, gfx);
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

bool MineField::Tile::reveal()
{
    if (state == State::Hidden)
    {
        state = State::Revealed;
        return true;
    }
    return false;
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
    gfx.DrawRect(boundary.GetExpanded(BORDER_WIDTH), Colors::Gray);
    gfx.DrawRect(boundary, Colors::White);
    for (int i = 0; i < nTiles; ++i)
    {
        minefield[i].draw(gfx, isMineTriggered);
    }
}

bool MineField::mouseIsWithinField(const Mouse& mouse)
{
    int tileSize = SpriteCodex::tileSize;
    return boundary.Contains(mouse.GetPos());
}

bool MineField::mineTriggered()
{
    return isMineTriggered;
}

bool MineField::allTilesRevealed()
{
    int nSafeTiles = (TILES_PER_HEIGHT * TILES_PER_WIDTH) - nMines;
    return (nSafeTiles == nRevealedSafeTiles);
}

void MineField::revealTile(const Vei2& pixelPos)
{
    const Vei2 gridPos{ pixelToGridPosition(pixelPos) };
    Tile& tile{ tileAt(gridPos) };
    if (tile.reveal())
    {
        if (tileAt(gridPos).hasMine)
        {
            isMineTriggered = true;
            return;
        }
        ++nRevealedSafeTiles;
        revealAdjacentSafeTiles(tile, 2);
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
	assert(boundary.Contains(pixelPos));
    // ACCOUNT FOR MARGIN OFFSET
    Vei2 modifiedPixelPos{ pixelPos - Vei2(MARGIN_LEFT, MARGIN_TOP) };
	return modifiedPixelPos / tileSize;
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

void MineField::revealAdjacentSafeTiles(const Tile& tile, int nTimes)
{
    if (nTimes == 0) return;

    int xStart = std::max(0, tile.gridPos.x - 1);
    int xEnd = std::min(TILES_PER_WIDTH - 1, tile.gridPos.x + 1);
    int yStart = std::max(0, tile.gridPos.y - 1);
    int yEnd = std::min(TILES_PER_HEIGHT - 1, tile.gridPos.y + 1);
    for (Vei2 gridPos = { xStart, yStart }; gridPos.y <= yEnd; ++gridPos.y)
    {
        for (gridPos.x = xStart; gridPos.x <= xEnd; ++gridPos.x)
        {
            Tile& tile = tileAt(gridPos);
            if (!tile.hasMine)
            {
                if (tile.reveal())
                {
                    ++nRevealedSafeTiles;
                    revealAdjacentSafeTiles(tile, nTimes - 1);
                }
            }
        }
    }
}

MineField::Tile& MineField::tileAt(const Vei2& gridPos)
{
    return minefield[gridPos.y * TILES_PER_WIDTH + gridPos.x];
}
