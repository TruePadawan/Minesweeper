#pragma once
#include "Graphics.h"
#include "Vei2.h"
#include "SpriteCodex.h"
#include "Mouse.h"
#include "RectI.h"

class MineField
{
public:
	MineField(int _nMines);
	void draw(Graphics& gfx);
	void revealTile(const Vei2& pixelPos);
	void flagTile(const Vei2& pixelPos);
	bool mouseIsWithinField(const Mouse& mouse);
	bool mineTriggered();
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
		void draw(Graphics& gfx, bool mineTriggered);
		void spawnMine();
		void reveal();
		void flag();
		void setNumberOfAdjacentMines(int count);
	private:
		Vei2 gridToPixelPosition(const Vei2& gridPos) const;
	public:
		Vei2 gridPos;
		State state;
		bool hasMine;
		int nAdjacentMines = -1;
	};
private:
	int getNumberOfAdjacentMines(const Tile& tile);
	Tile& tileAt(const Vei2& gridPos);
	Vei2 pixelToGridPosition(const Vei2& pixelPos) const;
private:
	static constexpr int TILES_PER_WIDTH = 20;
	static constexpr int TILES_PER_HEIGHT = 16;
	static constexpr int MARGIN_LEFT = 240;
	static constexpr int MARGIN_TOP = 172;
	static constexpr int BORDER_WIDTH = 10;
	RectI boundary;
	int nMines;
	bool isMineTriggered;
	Tile minefield[TILES_PER_WIDTH * TILES_PER_HEIGHT];
};

