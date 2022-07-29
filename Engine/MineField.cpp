#include "MineField.h"
#include <random>
#include <assert.h>

MineField::MineField(int _nMines)
{
    nMines = _nMines;
    for (int y = 0; y < TILES_PER_HEIGHT; ++y)
    {
        for (int x = 0; x < TILES_PER_WIDTH; ++x)
        {
            minefield[y * TILES_PER_WIDTH + x] = Tile({ x, y });
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
}

MineField::Tile::Tile(const Vei2& pos)
    :gridPos(pos), state(State::Hidden), hasMine(false)
{
}

void MineField::Tile::draw(Graphics& gfx)
{
    Vei2 pixelPos = gridToPixelPosition(gridPos);
    switch (state)
    {
    case MineField::Tile::State::Revealed:
        {
            if (hasMine)
            {
                SpriteCodex::DrawTile0(pixelPos, gfx);
                SpriteCodex::DrawTileBomb(pixelPos, gfx);
            }
            else {
                SpriteCodex::DrawTile0(pixelPos, gfx);
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

void MineField::draw(Graphics& gfx)
{
    int nTiles = TILES_PER_HEIGHT * TILES_PER_WIDTH;
    RectI background{ 0, 20 * 16, 0, 16 * 16 };
    gfx.DrawRect(background, Colors::White);

    for (int i = 0; i < nTiles; ++i)
    {
        minefield[i].draw(gfx);
    }
}

bool MineField::mouseIsWithinField(const Mouse& mouse)
{
    int tileSize = SpriteCodex::tileSize;
    Vei2 mouseGridPos{ mouse.GetPos() };
    return (mouseGridPos.x >= 0 && mouseGridPos.x < TILES_PER_WIDTH * tileSize &&
           mouseGridPos.y >= 0 && mouseGridPos.y < TILES_PER_HEIGHT * tileSize);
}

void MineField::revealTile(const Vei2& pixelPos)
{
    const Vei2 gridPos{ pixelToGridPosition(pixelPos) };
    minefield[gridPos.y * TILES_PER_WIDTH + gridPos.x].reveal();
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

Vei2 MineField::Tile::gridToPixelPosition(const Vei2& gridPos) const
{
    return gridPos * SpriteCodex::tileSize;
}

MineField::Tile& MineField::tileAt(const Vei2& gridPos)
{
    return minefield[gridPos.y * TILES_PER_WIDTH + gridPos.x];
}
