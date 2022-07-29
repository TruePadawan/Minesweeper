#pragma once
#include "Graphics.h"
#include "Vei2.h"
#include "SpriteCodex.h"
#include "Mouse.h"

class MineField
{
public:
	MineField(int _nMines);
	void draw(Graphics& gfx);
	void revealTile(const Vei2& pixelPos);
	void flagTile(const Vei2& pixelPos);
	bool mouseIsWithinField(const Mouse& mouse);
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
		void reveal();
		void flag();
	private:
		Vei2 gridToPixelPosition(const Vei2& gridPos) const;
	public:
		Vei2 gridPos;
		State state;
		bool hasMine;
	};
private:
	Tile& tileAt(const Vei2& gridPos);
	Vei2 pixelToGridPosition(const Vei2& pixelPos) const;
private:
	static constexpr int TILES_PER_WIDTH = 20;
	static constexpr int TILES_PER_HEIGHT = 16;
	int nMines;
	Tile minefield[TILES_PER_WIDTH * TILES_PER_HEIGHT];
};

