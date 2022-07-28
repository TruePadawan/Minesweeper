#pragma once
#include "Graphics.h"
#include "Vei2.h"
#include "SpriteCodex.h"

class MineField
{
public:
	MineField(int _nMines);
	void draw(Graphics& gfx);
	void reveal(const Vei2& gridPos);
private:
	class Tile
	{
	public:
		enum class State
		{
			Revealed, Flagged, Hidden
		};
	public:
		Tile() = default;
		Tile(const Vei2& pos);
		void draw(Graphics& gfx);
		void spawnMine();
	private:
		Vei2 gridToPixelPosition(const Vei2& gridPos) const;
	public:
		Vei2 gridPos;
		State state;
		bool hasMine;
	};
private:
	Tile& tileAt(const Vei2& gridPos);
private:
	static constexpr int TILE_PER_WIDTH = 20;
	static constexpr int TILE_PER_HEIGHT = 16;
	int nMines;
	Tile minefield[TILE_PER_WIDTH * TILE_PER_HEIGHT];
};

