#include "Dynamics.h"
#include "Assets.h"

//================================================================================================
//											Dynamic Creature - Javid (Player)
//================================================================================================

cDynamic_Creature_Javid::cDynamic_Creature_Javid() : cDynamic_Creature("Javid")
{
	bFriendly = true;
	nHealth = 10;
	nHealthMax = 10;
	nScore = 0;

	animations.mapStates["idle"].push_back(Assets::get().GetSprite("Javid_Idle"));

	animations.mapStates["run"].push_back(Assets::get().GetSprite("Javid_Run_1"));
	animations.mapStates["run"].push_back(Assets::get().GetSprite("Javid_Run_2"));
	animations.mapStates["run"].push_back(Assets::get().GetSprite("Javid_Run_3"));
	animations.mapStates["run"].push_back(Assets::get().GetSprite("Javid_Run_4"));

	animations.mapStates["brake"].push_back(Assets::get().GetSprite("Javid_Brake_1"));
	animations.mapStates["brake"].push_back(Assets::get().GetSprite("Javid_Brake_2"));
	animations.mapStates["brake"].push_back(Assets::get().GetSprite("Javid_Brake_3"));
	animations.mapStates["brake"].push_back(Assets::get().GetSprite("Javid_Brake_4"));
	animations.mapStates["brake"].push_back(Assets::get().GetSprite("Javid_Brake_5"));

	animations.mapStates["squat"].push_back(Assets::get().GetSprite("Javid_Squat"));

	animations.mapStates["jump"].push_back(Assets::get().GetSprite("Javid_Jump_1"));
	animations.mapStates["jump"].push_back(Assets::get().GetSprite("Javid_Jump_2"));


	animations.mapStates["fall"].push_back(Assets::get().GetSprite("Javid_Fall"));

	animations.mapStates["wipe"].push_back(Assets::get().GetSprite("Javid_Wipe_0"));
	animations.mapStates["wipe"].push_back(Assets::get().GetSprite("Javid_Wipe_1"));
	animations.mapStates["wipe"].push_back(Assets::get().GetSprite("Javid_Wipe_2"));
	animations.mapStates["wipe"].push_back(Assets::get().GetSprite("Javid_Wipe_3"));
	animations.mapStates["wipe"].push_back(Assets::get().GetSprite("Javid_Wipe_4"));


	animations.ChangeState("idle");

	pEquipedWeapon = (cWeapon*)Assets::get().GetItem("Wipe");
}

void cDynamic_Creature_Javid::PerformAttack()
{
	if (pEquipedWeapon == nullptr)
		return;

	animations.ChangeState("wipe", true);
	pEquipedWeapon->OnUse(this);
}

void cDynamic_Creature_Javid::UpdateAnimationState(float fElapsedTime)
{
	if ((animations.sCurrentState == "wipe" && animations.bCompletedAnimation) || animations.sCurrentState != "wipe")
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

	//Change animation speeds
	if (animations.sCurrentState == "run")
		animations.fTimeBetweenFrames = 0.1f * (10.0f / fabs(vx));
	else if (animations.sCurrentState == "brake")
		animations.fTimeBetweenFrames = 0.05f;
	else if (animations.sCurrentState == "wipe")
		animations.fTimeBetweenFrames = 0.07;
	else
		animations.fTimeBetweenFrames = 0.1f;
}
