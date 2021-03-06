#include "Items.h"
#include "Assets.h"
#include "olcPGEX_Sound.h"
#include "JvC_Engine.h"
#include <algorithm>

Platformer* cItem::g_engine = nullptr;

using namespace std;

cItem::cItem(std::string name, std::string desc)
{
	sName = name;
	sDescription = desc;
}

bool cItem::OnInteract(cDynamic* object)
{
	return false;
}

bool cItem::OnUse(cDynamic* object)
{
	return false;
}

void cItem::DrawSelf(olc::PixelGameEngine* pge, olc::GFX2D::Transform2D t)
{
	//In case we want to add any functionality here in sub classes
	animItem.DrawSelf(pge, t);
}

void cItem::Update(float fElapsedTime, cDynamic* player)
{
	//In case we want to add any functionality here in sub classes
	animItem.Update(fElapsedTime);
}

//================================================================================================
//											Health - Give player 10 hp
//================================================================================================
cItem_Health::cItem_Health() : 
	cItem("Health", "Restores 10 Health")
{
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Health"));
	animItem.ChangeState("default");
	bGravityApplies = true;
}

bool cItem_Health::OnInteract(cDynamic* object)
{
	OnUse(object);
	return false; //Just absorb - not add to inventory
}

bool cItem_Health::OnUse(cDynamic* object)
{
	if (object != nullptr)
	{
		cDynamic_Creature* dyn = (cDynamic_Creature*)object;
		dyn->nHealth = min(dyn->nHealth + 10, dyn->nHealthMax);
	}
	return true; //Item disappears - Used once
}


//================================================================================================
//											Health Boost - Raise player max hp by 10
//================================================================================================
cItem_HealthBoost::cItem_HealthBoost() : 
	cItem("Health Boost", "Increases Max Health by 10")
{
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Health Boost"));
	animItem.ChangeState("default");
	bGravityApplies = true;
}

bool cItem_HealthBoost::OnInteract(cDynamic* object)
{
	return true; // Just add to inventory
}

bool cItem_HealthBoost::OnUse(cDynamic* object)
{
	if (object != nullptr)
	{
		cDynamic_Creature* dyn = (cDynamic_Creature*)object;
		dyn->nHealthMax += 10;
		dyn->nHealth = dyn->nHealthMax;
	}

	return true; // Remove from inventory
}

//================================================================================================
//											Flames Cash - Increase score by 10
//================================================================================================
cItem_FlamesCash::cItem_FlamesCash() :
	cItem("Flames Cash", "Increases Score by 10")
{
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Money_00"));
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Money_01"));
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Money_02"));
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Money_03"));
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Money_04"));
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Money_05"));
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Money_06"));
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Money_07"));
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Money_08"));
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Money_09"));
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Money_10"));
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Money_11"));
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Money_12"));
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Money_13"));
	animItem.ChangeState("default");

	bGravityApplies = false;
}

bool cItem_FlamesCash::OnInteract(cDynamic* object)
{
	OnUse(object);
	return false;
}

bool cItem_FlamesCash::OnUse(cDynamic* object)
{
	((cDynamic_Creature_Javid*)object)->nScore += 10;
	olc::SOUND::PlaySample(Assets::get().GetSound("sndSampleB")); // Plays Sample B
	return true;
}

//==============================================================================================================
//											Weapon - Base weapon class (Like reg item, just stores damage)
//==============================================================================================================
cWeapon::cWeapon(string name, string desc, int dmg) : cItem(name, desc)
{
	nDamage = dmg;
}

bool cWeapon::OnInteract(cDynamic* object)
{
	return false;
}

bool cWeapon::OnUse(cDynamic* object)
{
	return false;
}

//================================================================================================
//											Weapon - Covid Projectile
//================================================================================================
cWeapon_Covid_Projectile::cWeapon_Covid_Projectile() :
	cWeapon("Covid Ball", "Deadly Covid Attack - 1 dmg", 1)
{
	//Add sprites here!
	// Weapon Model Sprites (Can make him hold weapon later)
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Projectile_00"));
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Projectile_01"));
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Projectile_02"));
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Projectile_03"));
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Projectile_04"));
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Projectile_05"));
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Projectile_06"));
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Projectile_07"));

	animItem.ChangeState("default");

	// Projectile Sprites
	animProjectile.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Projectile_00"));
	animProjectile.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Projectile_01"));
	animProjectile.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Projectile_02"));
	animProjectile.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Projectile_03"));
	animProjectile.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Projectile_04"));
	animProjectile.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Projectile_05"));
	animProjectile.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Projectile_06"));
	animProjectile.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Projectile_07"));

	animProjectile.ChangeState("default");
}

bool cWeapon_Covid_Projectile::OnUse(cDynamic* object)
{
	// When weapons are used, they are used on the object that owns the weapon, i.e.
	// the attacker. However this does not imply the attacker attacks themselves

	//Get attacker
	cDynamic_Creature* aggressor = (cDynamic_Creature*)object;

	// Determine attack origin
	float x, y, vx, vy;
	if (aggressor->fFaceDir == cDynamic_Creature::LEFT)
	{
		x = aggressor->px - 1.0f;
		y = aggressor->py;
		vx = -1.0f; vy = 0.0f;
	}

	if (aggressor->fFaceDir == cDynamic_Creature::RIGHT)
	{
		x = aggressor->px + 1.0f;
		y = aggressor->py;
		vx = 1.0f; vy = 0.0f;
	}

	cDynamic_Projectile* p = new cDynamic_Projectile(aggressor->px - 1.0f, y, aggressor->bFriendly, -1.0f * 5.0f, vy, 0.5f, animProjectile, cDynamic_Creature::LEFT);
	p->bSolidVsMap = true;
	p->bSolidVsDynamic = false; //Needs to be false for projectile collision to execute
	p->nDamage = this->nDamage;
	p->bOneHit = true;
	p->fKnockBackVel = 4.0f;
	p->fKnockBackDuration = 0.1f;

	cDynamic_Projectile* p2 = new cDynamic_Projectile(aggressor->px + 1.0f, y, aggressor->bFriendly, 1.0f * 5.0f, vy, 0.5f, animProjectile, cDynamic_Creature::RIGHT);
	p2->bSolidVsMap = true;
	p2->bSolidVsDynamic = false;
	p2->nDamage = this->nDamage;
	p2->bOneHit = true;
	p2->fKnockBackVel = 4.0f;
	p2->fKnockBackDuration = 0.1f;

	g_engine->AddProjectile(p);
	g_engine->AddProjectile(p2);


	return false;
}

//================================================================================================
//											"Weapon" - Covid Touch Damage Profile
//================================================================================================
cWeapon_Covid_Touch::cWeapon_Covid_Touch() :
	cWeapon("Covid Touch", "Consequence for touching covid enemy - 1 dmg", 1)
{
	//Add sprites here!
	// Weapon Model Sprites (Can make him hold weapon later)
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Blank22x22"));


	animItem.ChangeState("default");

	// Projectile Sprites
	//animProjectile.mapStates["default"].push_back(Assets::get().GetSprite("Blank22x22"));
	animProjectile.mapStates["default"].push_back(Assets::get().GetSprite("Blank22x22"));


	animProjectile.ChangeState("default");
}

bool cWeapon_Covid_Touch::OnUse(cDynamic* object)
{
	// When weapons are used, they are used on the object that owns the weapon, i.e.
	// the attacker. However this does not imply the attacker attacks themselves

	//Get attacker
	cDynamic_Creature* aggressor = (cDynamic_Creature*)object;

	cDynamic_Projectile* p = new cDynamic_Projectile(aggressor->px + (1/22.0f) /*one pixel*/, aggressor->py + (1/22.0f), aggressor->bFriendly, aggressor->vx, 0.0f, 0.1f, animProjectile, cDynamic_Creature::LEFT);
	p->bSolidVsMap = false;
	p->bSolidVsDynamic = false; //Needs to be false for projectile collision to execute
	p->nDamage = this->nDamage;
	p->bOneHit = true;
	p->fKnockBackVel = 4.0f;
	p->fKnockBackDuration = 0.1f;
	p->KnockBackMode = both_omnidirectional;
	p->SetDimension(aggressor->GetDimension() + 2); // Encapsulate projectile around object, so it will hit whoever ran into it

	g_engine->AddProjectile(p);


	return false;
}


//================================================================================================
//											Weapon - Clorox wipe
//================================================================================================
cWeapon_Wipe::cWeapon_Wipe() :
	cWeapon("Wipe", "A Clorox Wipe - One of the last of its \nkind", 1)
{
	//Add sprites here!
	// Weapon Model Sprites (Can make him hold weapon later)
	animItem.mapStates["default"].push_back(Assets::get().GetSprite("Javid_Wipe_3"));
	animItem.ChangeState("default");

	// Projectile Sprites
	animProjectile.mapStates["default"].push_back(Assets::get().GetSprite("Blank22x22"));

	animProjectile.ChangeState("default");
}

bool cWeapon_Wipe::OnUse(cDynamic* object)
{
	// When weapons are used, they are used on the object that owns the weapon, i.e.
	// the attacker. However this does not imply the attacker attacks themselves

	//Get direction of attacker
	cDynamic_Creature* aggressor = (cDynamic_Creature*)object;

	// Determine attack origin
	float x, y, vx, vy;
	if (aggressor->fFaceDir == cDynamic_Creature::LEFT)
	{
		x = aggressor->px - 0.6f; //Not 1.0f in order for vector calculations in JvC_Engine::Damage() to not be zero (sort of a hack)
		y = aggressor->py;
		vx = -1.0f; vy = 0.0f;
	}

	if (aggressor->fFaceDir == cDynamic_Creature::RIGHT)
	{
		x = aggressor->px + 0.6f;
		y = aggressor->py;
		vx = 1.0f; vy = 0.0f;
	}

	//if (aggressor->nHealth == aggressor->nHealthMax)
	//{
	//	// Beam Wipe
	//	cDynamic_Projectile* p = new cDynamic_Projectile(x, y, aggressor->bFriendly, vx * 15.0f, vy * 15.0f, 1.0f, animProjectile, aggressor->fFaceDir);
	//	p->bSolidVsMap = true;
	//	p->bSolidVsDynamic = false;
	//	p->nDamage = this->nDamage; //5
	//	p->bOneHit = true;
	//	p->fKnockBackVel = 4.0f;
	//	p->fKnockBackDuration = 0.3f;

	//	g_engine->AddProjectile(p);
	//}

	cDynamic_Projectile* p = new cDynamic_Projectile(x, y + 0.5f, aggressor->bFriendly, aggressor->vx, aggressor->vy - 8.0f, 0.1f, animProjectile, aggressor->fFaceDir);
	p->bSolidVsMap = false;
	p->bSolidVsDynamic = false;
	p->nDamage = this->nDamage;
	p->bOneHit = true;
	p->fKnockBackVel = 7.0f;
	p->KnockBackMode = both;

	g_engine->AddProjectile(p);

	return false;
}