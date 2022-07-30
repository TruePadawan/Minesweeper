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
	bool allTilesRevealed();
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
		bool reveal();
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
	static constexpr int TILES_PER_WIDTH = 6;
	static constexpr int TILES_PER_HEIGHT = 6;
	static constexpr int MARGIN_LEFT = (Graphics::ScreenWidth/2) - ((TILES_PER_WIDTH * SpriteCodex::tileSize)/2);
	static constexpr int MARGIN_TOP = (Graphics::ScreenHeight/2) - ((TILES_PER_HEIGHT * SpriteCodex::tileSize)/2);
	static constexpr int BORDER_WIDTH = 10;
private:
	int nRevealedSafeTiles;
	RectI boundary;
	int nMines;
	bool isMineTriggered;
	Tile minefield[TILES_PER_WIDTH * TILES_PER_HEIGHT];
};

