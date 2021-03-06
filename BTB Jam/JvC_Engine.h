#pragma once

//#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
//#define OLC_PGEX_SOUND
#include "olcPGEX_Sound.h"
//#define OLC_PGE_GRAPHICS2D
#include "olcPGEX_Graphics2D.h"

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
using namespace std;

#include "Assets.h"
#include "Animator.h" //Don't think I need this when finally get rid of all left over stuff
#include "Maps.h"
#include "Dynamics.h"
#include "Commands.h"
#include "Quests.h"
#include "Items.h"

class Platformer : public olc::PixelGameEngine
{
public:
	Platformer();

private:
	//Level Storage
	cMap* pCurrentMap = nullptr;

	//Player Properties
	cDynamic_Creature_Javid* m_pPlayer = nullptr;

	vector<cDynamic*> vecDynamics; // Fixed - Changed depending on quest and maps
	vector<cDynamic*> vecProjectiles; // Transient - Can be added and removed easily (only of type cDynamic b/c we want to also respond to physics engine like vecDynamics w/o duplicating code
	cScriptProcessor m_script;

	list<cQuest*> listQuests;
	list<cItem*> listItems;

	//Camera Properties
	float fCameraPosX = 0.0f;
	float fCameraPosY = 0.0f;
	bool bPanCamera = true;

	enum
	{
		MODE_TITLE,
		MODE_LOCAL_MAP,
		MODE_WORLD_MAP,
		MODE_INVENTORY,
		MODE_SHOP,
		MODE_GAMEOVER
	};

	int nGameMode = MODE_TITLE;

	/*==============INVENTORY===============*/
	int8_t nInvSelectX = 0; //NEEDS to be signed (so it can go to -1 in the inventory update fcn)
	int8_t nInvSelectY = 0;
	struct InvSelect
	{
		InvSelect(uint8_t x, uint8_t y, uint8_t a) { this->x = x; this->y = y, alpha = a; }
		uint8_t x;
		uint8_t y;
		uint8_t alpha;
	};
	std::vector<InvSelect*> vecInvSelect;

	olc::Pixel inventoryColor;
	olc::Pixel rectColor;

	float fInvStateTick; //For inventory background fading
	/*======================================*/

	/*=============TITLE====================*/
	int nSequenceCnt = 0;
	float fTitleStateTick = 3.0f;
	/*======================================*/

	olc::Sprite* backBuff; //Save state of current frame for background of transparent inventory screen

	//Sprite Resources
	olc::Sprite* spriteFloor = nullptr;
	olc::Sprite* spriteBrick = nullptr;
	olc::Sprite* spriteTEST = nullptr;

	//Sprite selection flags
	//int nObjectWidth = 22; //Maybe make these global constants in the future in case we want to change resolution, or do other things?
	//int nObjectHeight = 22;

	//Sprite Animation Class
	//cAnimator animPlayer;
	cAnimator animMoney;

	//Pickups
#define COIN L'o'
#define TEST L'1'

	//Pickup variables
	//bool bPickupCollected = false;
	string sScoreString;

	bool bGamePaused = false;

	//Debug + Testing
	bool showDebug = true;
	


public:
	bool OnUserCreate() override;
	bool OnUserDestroy();
	bool OnUserUpdate(float fElapsedTime) override;

	bool UpdateTitleScreen(float fElapsedTime);
	bool UpdateLocalMap(float fElapsedTime);
	//bool UpdateWorldMap(float fElapsedTime);
	bool UpdateInventory(float fElapsedTime);
	//bool UpdateShop(float fElapsedTime);
	bool UpdateGameOver(float fElapsedTime);

	void ShowDialog(vector<string> vecLines, olc::Pixel color = olc::DARK_BLUE);
	void DisplayDialog(vector<string> vecLines, int x, int y);
	void ChangeMap(string sMapName, float x, float y);
	void AddQuest(cQuest* quest);
	void PanCamera(bool b);

	void AddProjectile(cDynamic_Projectile* proj);

	//Inventory handling convenience
	bool GiveItem(cItem* item);
	bool TakeItem(cItem* item);
	bool HasItem(cItem* item);

	void Attack(cDynamic_Creature* aggressor, cWeapon* weapon);
	void Damage(cDynamic_Projectile* projectile, cDynamic_Creature* victim);

public:
	vector<string> vecDialogToShow;
	bool bShowDialog = false;
	olc::Pixel dialogColor = olc::DARK_BLUE;
};
