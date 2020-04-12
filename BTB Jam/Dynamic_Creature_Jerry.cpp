#include "Dynamics.h"
#include "Assets.h"

//================================================================================================
//											Dynamic Creature - Jerry (Player)
//================================================================================================

cDynamic_Creature_Jerry::cDynamic_Creature_Jerry() : cDynamic_Creature("Jerry")
{
	bFriendly = true;
	nHealth = 10;
	nHealthMax = 10;
	nScore = 0;

	animations.mapStates["idle"].push_back(Assets::get().GetSprite("Jerry_Idle"));

	animations.mapStates["run"].push_back(Assets::get().GetSprite("Jerry_Run_1"));
	animations.mapStates["run"].push_back(Assets::get().GetSprite("Jerry_Run_2"));
	animations.mapStates["run"].push_back(Assets::get().GetSprite("Jerry_Run_3"));
	animations.mapStates["run"].push_back(Assets::get().GetSprite("Jerry_Run_4"));

	animations.mapStates["brake"].push_back(Assets::get().GetSprite("Jerry_Brake_1"));
	animations.mapStates["brake"].push_back(Assets::get().GetSprite("Jerry_Brake_2"));
	animations.mapStates["brake"].push_back(Assets::get().GetSprite("Jerry_Brake_3"));
	animations.mapStates["brake"].push_back(Assets::get().GetSprite("Jerry_Brake_4"));
	animations.mapStates["brake"].push_back(Assets::get().GetSprite("Jerry_Brake_5"));

	animations.mapStates["squat"].push_back(Assets::get().GetSprite("Jerry_Squat"));

	animations.mapStates["jump"].push_back(Assets::get().GetSprite("Jerry_Jump_1"));
	animations.mapStates["jump"].push_back(Assets::get().GetSprite("Jerry_Jump_2"));


	animations.mapStates["fall"].push_back(Assets::get().GetSprite("Jerry_Fall"));

	animations.mapStates["wipe"].push_back(Assets::get().GetSprite("Jerry_Shoot_0"));
	animations.mapStates["wipe"].push_back(Assets::get().GetSprite("Jerry_Shoot_1"));
	animations.mapStates["wipe"].push_back(Assets::get().GetSprite("Jerry_Shoot_2"));
	animations.mapStates["wipe"].push_back(Assets::get().GetSprite("Jerry_Shoot_3"));
	animations.mapStates["wipe"].push_back(Assets::get().GetSprite("Jerry_Shoot_4"));


	animations.ChangeState("idle");

	pEquipedWeapon = (cWeapon*)Assets::get().GetItem("Basic Sword");
}

void cDynamic_Creature_Jerry::PerformAttack()
{
	if (pEquipedWeapon == nullptr)
		return;

	animations.ChangeState("wipe", true);
	pEquipedWeapon->OnUse(this);
}

void cDynamic_Creature_Jerry::UpdateAnimationState(float fElapsedTime)
{
	if (animations.sCurrentState != "wipe" || animations.sCurrentState != "shoot")
	{
		if (bObjectOnGround)
		{
			if (fabs(vx) == 0.0f)
			{
				animations.ChangeState("idle");
			}
			else if (fFaceDir == RIGHT && vx < 0 || fFaceDir == LEFT && vx > 0) //Just changed direction but still moving the opposite way -> braking
			{
				animations.ChangeState("brake");
			}
			else
			{
				animations.ChangeState("run");
			}

			if (bSquat) //This is changed in the Input Handling section
				animations.ChangeState("squat");
		}
		else
		{
			if (vy <= 0)
			{
				animations.ChangeState("jump", true);
			}
			else
			{
				animations.ChangeState("fall");
			}
		}
	}
}
