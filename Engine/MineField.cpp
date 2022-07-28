#include "MineField.h"
#include <random>
#include <assert.h>

MineField::MineField(int _nMines)
{
    nMines = _nMines;
    for (int y = 0; y < TILE_PER_HEIGHT; ++y)
    {
        for (int x = 0; x < TILE_PER_WIDTH; ++x)
        {
            minefield[y * TILE_PER_WIDTH + x] = Tile({ x, y });
        }
    }

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> xDist(0, TILE_PER_WIDTH - 1);
    std::uniform_int_distribution<int> yDist(0, TILE_PER_HEIGHT - 1);

	for (int i = 0; i < nMines; ++i)
	{
		Vei2 gridPos = { 0,0 };
		do {
			gridPos = { xDist(rng), yDist(rng) };
		} while (tileAt(gridPos).hasMine());
		tileAt(gridPos).spawnMine();
	}
}

MineField::Tile::Tile(const Vei2& pos)
    :gridPos(pos), state(State::Hidden), mine(false)
{
}

void MineField::Tile::draw(Graphics& gfx)
{
    Vei2 pixelPos = gridToPixelPosition(gridPos);
    switch (state)
    {
    case MineField::Tile::State::Revealed:
        {
            if (mine)
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

bool MineField::Tile::hasMine()
{
    return mine;
}

void MineField::Tile::spawnMine()
{
    assert(!hasMine());
    mine = true;
}

void MineField::draw(Graphics& gfx)
{
    int nTiles = TILE_PER_HEIGHT * TILE_PER_WIDTH;
    RectI background{ 0, 20 * 16, 0, 16 * 16 };
    gfx.DrawRect(background, Colors::White);

    for (int i = 0; i < nTiles; ++i)
    {
        minefield[i].draw(gfx);
    }
}

Vei2 MineField::Tile::gridToPixelPosition(const Vei2& gridPos) const
{
    return gridPos * SpriteCodex::tileSize;
}

MineField::Tile& MineField::tileAt(const Vei2& gridPos)
{
    return minefield[gridPos.y * TILE_PER_WIDTH + gridPos.x];
}
