#include "Tiles.h"
#include "Assets.h"

using namespace std;


//==================================================================================================================================================================================================
//																					BASE TILE
//==================================================================================================================================================================================================
cTile::cTile(string tileName, float x, float y, bool solid)
{
	sName = tileName;
	this->solid = solid;
	broken = false;

	px = x;
	py = y;
}

//olc::Sprite* cTile::GetCurrentFrame()
//{
//	return animTile.mapStates[animTile.sCurrentState][animTile.nCurrentFrame];
//}


void cTile::DrawSelf(olc::PixelGameEngine* pge, float screenPosX, float screenPosY) //screen space coordinates
{
	if (!broken)
	{
		olc::GFX2D::Transform2D t;
		t.Translate(screenPosX, screenPosY);
		animTile.DrawSelf(pge, t);
	}
}

//void cTile::DrawSelf(olc::PixelGameEngine* pge, olc::GFX2D::Transform2D& t)
//{
//	animTile.DrawSelf(pge, t);
//}


//==================================================================================================================================================================================================
//																					FLOOR
//==================================================================================================================================================================================================

cTile_Floor::cTile_Floor(float x, float y) : cTile("floor", x, y, true)
{
	animTile.mapStates["normal"].push_back(Assets::get().GetSprite("Floor"));
	animTile.ChangeState("normal");
}

//==================================================================================================================================================================================================
//																					BRICK
//==================================================================================================================================================================================================
cTile_Brick::cTile_Brick(float x, float y) : cTile("brick", x, y, true)
{
	animTile.mapStates["default"].push_back(Assets::get().GetSprite("Brick"));

	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_01"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_02"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_03"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_04"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_05"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_06"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_07"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_08"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_09"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_10"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_11"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_12"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_13"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_14"));

	animTile.ChangeState("default");
}

void cTile_Brick::Update(float fElapsedTime, cDynamic* player)
{
	//if (animTile.sCurrentState == "break" && animTile.bCompletedAnimation) //If last sprite in animation is blank... don't need this
	//	broken = true;

	if (animTile.sCurrentState == "break")
		animTile.fTimeBetweenFrames = 0.02f;
	
	animTile.Update(fElapsedTime);
}

void cTile_Brick::OnPunch()
{
	animTile.ChangeState("break", true);
	solid = false;
}

//==================================================================================================================================================================================================
//																					SKY
//==================================================================================================================================================================================================
cTile_Sky::cTile_Sky(float x, float y) : cTile("sky", x, y, false)
{
}

//==================================================================================================================================================================================================
//																					COLOR TILE
//==================================================================================================================================================================================================
cTile_Color::cTile_Color(float x, float y, olc::Pixel color) : cTile("color_tile", x, y, false)
{
	tileColor = color;
}

void cTile_Color::DrawSelf(olc::PixelGameEngine* pge, float screenPosX, float screenPosY)
{
	pge->FillRect(screenPosX, screenPosY, 22, 22, tileColor); //22 hard coded in, but is a variable in main game function, but this likely won't change...
}

//==================================================================================================================================================================================================
//																					INVISIBLE BOUNDARY
//==================================================================================================================================================================================================
cTile_Invisible_Boundary::cTile_Invisible_Boundary(float x, float y) : cTile("invisble_wall", x, y, true)
{
}

//==================================================================================================================================================================================================
//																					GATE BLOCK - When player moves past block, becomes solid
//==================================================================================================================================================================================================
cTile_Gate::cTile_Gate(float x, float y) : cTile("gate", x, y, false)
{
	//Assets here
	animTile.mapStates["default"].push_back(Assets::get().GetSprite(" "));

	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_14"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_13"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_12"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_11"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_10"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_09"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_08"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_07"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_06"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_05"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_04"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_03"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_02"));
	animTile.mapStates["break"].push_back(Assets::get().GetSprite("Brick_Break_01"));

	animTile.ChangeState("default");

}

void cTile_Gate::Update(float fElapsedTime, cDynamic* player)
{
	if (player->py + 0.9f < py) //When above block, will become solid blocking player from falling back down
	{
		animTile.ChangeState("break", true);
		animTile.fTimeBetweenFrames = 0.02f;

		solid = true;
	}

	animTile.Update(fElapsedTime);
}