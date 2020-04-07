#include "Assets.h"

using namespace std;

Assets::Assets()
{
}

Assets::~Assets()
{
}

void Assets::LoadSprites() //Single one time load of all sprite resources
{
	auto load = [&](string name, string filename)
	{
		mapSprites[name] = new olc::Sprite(filename);
	};

	//Floor
	load("Floor", "../Sprites/Floor.png");

	//Brick
	load("Brick", "../Sprites/Brick/Brick.png");
	load("Brick_Break_01", "../Sprites/Brick/Brick_Break_00.png");
	load("Brick_Break_02", "../Sprites/Brick/Brick_Break_01.png");
	load("Brick_Break_03", "../Sprites/Brick/Brick_Break_02.png");
	load("Brick_Break_04", "../Sprites/Brick/Brick_Break_03.png");
	load("Brick_Break_05", "../Sprites/Brick/Brick_Break_04.png");
	load("Brick_Break_06", "../Sprites/Brick/Brick_Break_05.png");
	load("Brick_Break_07", "../Sprites/Brick/Brick_Break_06.png");
	load("Brick_Break_08", "../Sprites/Brick/Brick_Break_07.png");
	load("Brick_Break_09", "../Sprites/Brick/Brick_Break_08.png");
	load("Brick_Break_10", "../Sprites/Brick/Brick_Break_09.png");
	load("Brick_Break_11", "../Sprites/Brick/Brick_Break_10.png");
	load("Brick_Break_12", "../Sprites/Brick/Brick_Break_11.png");
	load("Brick_Break_13", "../Sprites/Brick/Brick_Break_12.png");
	load("Brick_Break_14", "../Sprites/Brick/Brick_Break_13.png");


	//Jerry
	load("Jerry_Idle", "../Sprites/Jerry_Idle.png");

	load("Jerry_Run_1", "../Sprites/Jerry_Run_1.png");
	load("Jerry_Run_2", "../Sprites/Jerry_Run_2.png");
	load("Jerry_Run_3", "../Sprites/Jerry_Run_3.png");
	load("Jerry_Run_4", "../Sprites/Jerry_Run_4.png");

	load("Jerry_Brake_1", "../Sprites/Jerry_Brake_1.png");
	load("Jerry_Brake_2", "../Sprites/Jerry_Brake_2.png");
	load("Jerry_Brake_3", "../Sprites/Jerry_Brake_3.png");
	load("Jerry_Brake_4", "../Sprites/Jerry_Brake_4.png");
	load("Jerry_Brake_5", "../Sprites/Jerry_Brake_5.png");

	load("Jerry_Squat", "../Sprites/Jerry_Squat.png");

	load("Jerry_Jump_1", "../Sprites/Jerry_Jump_1.png");
	load("Jerry_Jump_2", "../Sprites/Jerry_Jump_2.png");

	load("Jerry_Fall", "../Sprites/Jerry_Fall.png");

	 //Money
	 load("Money_00", "../Sprites/Money/Money_00.png");
	 load("Money_01", "../Sprites/Money/Money_01.png");
	 load("Money_02", "../Sprites/Money/Money_02.png");
	 load("Money_03", "../Sprites/Money/Money_03.png");
	 load("Money_04", "../Sprites/Money/Money_04.png");
	 load("Money_05", "../Sprites/Money/Money_05.png");
	 load("Money_06", "../Sprites/Money/Money_06.png");
	 load("Money_07", "../Sprites/Money/Money_07.png");
	 load("Money_08", "../Sprites/Money/Money_08.png");
	 load("Money_09", "../Sprites/Money/Money_09.png");
	 load("Money_10", "../Sprites/Money/Money_10.png");
	 load("Money_11", "../Sprites/Money/Money_11.png");
	 load("Money_12", "../Sprites/Money/Money_12.png");
	 load("Money_13", "../Sprites/Money/Money_13.png");

	 // Enemies
	 load("Covid_Small_00", "../Sprites/Covid_Small/sprite_00.png");
	 load("Covid_Small_01", "../Sprites/Covid_Small/sprite_01.png");
	 load("Covid_Small_02", "../Sprites/Covid_Small/sprite_02.png");
	 load("Covid_Small_03", "../Sprites/Covid_Small/sprite_03.png");
	 load("Covid_Small_04", "../Sprites/Covid_Small/sprite_04.png");
	 load("Covid_Small_05", "../Sprites/Covid_Small/sprite_05.png");
	 load("Covid_Small_06", "../Sprites/Covid_Small/sprite_06.png");
	 load("Covid_Small_07", "../Sprites/Covid_Small/sprite_07.png");
	 load("Covid_Small_08", "../Sprites/Covid_Small/sprite_08.png");
	 load("Covid_Small_09", "../Sprites/Covid_Small/sprite_09.png");
	 load("Covid_Small_10", "../Sprites/Covid_Small/sprite_10.png");
	 load("Covid_Small_11", "../Sprites/Covid_Small/sprite_11.png");
	 load("Covid_Small_12", "../Sprites/Covid_Small/sprite_12.png");
	 load("Covid_Small_13", "../Sprites/Covid_Small/sprite_13.png");
	 load("Covid_Small_14", "../Sprites/Covid_Small/sprite_14.png");
	 load("Covid_Small_15", "../Sprites/Covid_Small/sprite_15.png");
	 load("Covid_Small_16", "../Sprites/Covid_Small/sprite_16.png");
	 load("Covid_Small_17", "../Sprites/Covid_Small/sprite_17.png");

	 load("Covid_Medium_00", "../Sprites/Covid_Medium/sprite_00.png");
	 load("Covid_Medium_01", "../Sprites/Covid_Medium/sprite_01.png");
	 load("Covid_Medium_02", "../Sprites/Covid_Medium/sprite_02.png");
	 load("Covid_Medium_03", "../Sprites/Covid_Medium/sprite_03.png");
	 load("Covid_Medium_04", "../Sprites/Covid_Medium/sprite_04.png");
	 load("Covid_Medium_05", "../Sprites/Covid_Medium/sprite_05.png");
	 load("Covid_Medium_06", "../Sprites/Covid_Medium/sprite_06.png");
	 load("Covid_Medium_07", "../Sprites/Covid_Medium/sprite_07.png");
	 load("Covid_Medium_08", "../Sprites/Covid_Medium/sprite_08.png");
	 load("Covid_Medium_09", "../Sprites/Covid_Medium/sprite_09.png");
	 load("Covid_Medium_10", "../Sprites/Covid_Medium/sprite_10.png");
	 load("Covid_Medium_11", "../Sprites/Covid_Medium/sprite_11.png");
	 load("Covid_Medium_12", "../Sprites/Covid_Medium/sprite_12.png");
	 load("Covid_Medium_13", "../Sprites/Covid_Medium/sprite_13.png");
	 load("Covid_Medium_14", "../Sprites/Covid_Medium/sprite_14.png");



	 //Random
	 load("Hi_Mom", "../Sprites/Hi_Mom.png");
	 load("Health", "../Sprites/Health.png");
	 load("Health Boost", "../Sprites/Health_Boost.png");
}

void Assets::LoadMaps()
{
	auto load = [&](cMap* map)
	{
		mapMaps[map->sName] = map;
	};

	load(new cMap_Level1);
	load(new cMap_Level2);
}

void Assets::LoadItems()
{
	auto load = [&](cItem* item, int id = 0)
	{
		mapItems[item->sName] = id;
	};

	load(new cItem_Health, 1);
	load(new cItem_HealthBoost, 2);
	load(new cItem_FlamesCash, 3);
	load(new cWeapon_Sword, 4);
	load(new cWeapon_Pistol, 5);
	load(new cWeapon_Covid_Medium, 6);

}

void Assets::LoadSounds()
{
	auto load = [&](string name, string filename)
	{
		mapSounds[name] = olc::SOUND::LoadAudioSample(filename);
	};

	load("sndSampleA","../Sounds/SampleA.wav");
	load("sndSampleB","../Sounds/SampleB.wav");
	load("sndSampleC","../Sounds/SampleC.wav");
	load("sndBoo","../Sounds/Boo.wav");
	load("sndWuWuWu","../Sounds/WuWuWu.wav");
	load("sndGetMoney","../Sounds/GetMoney.wav");
	load("LitLoop", "../Sounds/LIIIIIIT.wav");
}

