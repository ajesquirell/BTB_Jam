#include "Dynamics.h"
#include "Items.h"
#include "Assets.h" //One we want to change to match how he does his dependencies


cDynamic_Creature_Covid_Small::cDynamic_Creature_Covid_Small() : cDynamic_Creature("Covid_Small")
{
	bFriendly = false;
	nHealth = 2;
	nHealthMax = 2;
	fStateTick = 2.0f;
	nDimensions = 10;

	animations.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Small_00"));
	animations.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Small_01"));
	animations.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Small_02"));
	animations.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Small_03"));
	animations.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Small_04"));
	animations.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Small_05"));
	animations.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Small_06"));
	animations.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Small_07"));
	animations.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Small_08"));
	animations.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Small_09"));
	animations.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Small_10"));
	animations.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Small_11"));
	animations.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Small_12"));
	animations.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Small_13"));
	animations.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Small_14"));
	animations.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Small_15"));
	animations.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Small_16"));
	animations.mapStates["default"].push_back(Assets::get().GetSprite("Covid_Small_17"));

	animations.mapStates["jump"].push_back(Assets::get().GetSprite("Covid_Small_00"));


	animations.ChangeState("default");

	pTouchDamageWeapon = (cWeapon*)Assets::get().GetItem("Covid Touch");
}

void cDynamic_Creature_Covid_Small::Behavior(float fElapsedTime, cDynamic* player)
{
	// Check if player is nearby
	float fTargetX = player->px - px;
	float fTargetY = player->py - py;
	float fDistance = sqrtf(fTargetX * fTargetX + fTargetY * fTargetY);

	//fDirectionX //Playing around with making some of these variables static does some pretty interesting things
	
	

	fStateTick -= fElapsedTime;
	if (fStateTick <= 0.0f)
	{
		if (fDistance < 8.0f && fDistance != 0) // Avoid divide by 0 error that freezes program (when we had vx = (fTargetX / fDistance) * 2.0f;)
		{
			if (fTargetX < 0.0f)
				fFaceDir = LEFT; // -1.0f
			else
				fFaceDir = RIGHT; // 1.0f

			if (bObjectOnGround)
			{
				vy = -6.0f; //Jump
				vx = fFaceDir * 1.0f;
			}
		}
		else
		{
			vx = 0;
			vy = 0;
		}

		fStateTick += 1.8f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.5f - 1.8f))); //Jump at random times between 1.8 and 2.5 seconds
	}
}

void cDynamic_Creature_Covid_Small::PerformAttack()
{
	if (pEquipedWeapon == nullptr)
		return;

	pEquipedWeapon->OnUse(this);
}

bool cDynamic_Creature_Covid_Small::OnInteract(cDynamic* player)
{
	if (pTouchDamageWeapon != nullptr)
		pTouchDamageWeapon->OnUse(this);

	return false;
}

void cDynamic_Creature_Covid_Small::UpdateAnimationState(float fElapsedTime)
{
	if (!bObjectOnGround)
		animations.ChangeState("jump");
	else
		animations.ChangeState("default");
}