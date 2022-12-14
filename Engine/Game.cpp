/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	field(8)
{
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	if (field.mineTriggered() || field.allTilesRevealed()) return;
	// ONLY ATTEMPT TO REVEAL A TILE WHEN THE MOUSE WAS CLICKED INSIDE OF THE MINEFIELD
	if (!wnd.mouse.IsEmpty() && field.mouseIsWithinField(wnd.mouse) == true)
	{
		const Mouse::Event ev = wnd.mouse.Read();		
		if (ev.GetType() == Mouse::Event::Type::LPress)
		{
			field.revealTile(wnd.mouse.GetPos());
		}
		else if (ev.GetType() == Mouse::Event::Type::RPress)
		{
			field.flagTile(wnd.mouse.GetPos());
		}
	}
}

void Game::ComposeFrame()
{
	field.draw(gfx);
	if (field.allTilesRevealed()) SpriteCodex::DrawWin(Vei2(400, 150), gfx);
}
