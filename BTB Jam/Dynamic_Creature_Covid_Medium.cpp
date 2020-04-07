#include "Dynamics.h"
#include "Items.h"
#include "Assets.h" //One we want to change to match how he does his dependencies


cDynamic_Creature_Covid_Medium::cDynamic_Creature_Covid_Medium() : cDynamic_Creature("Covid_Medium")
{
	bFriendly = false;
	nHealth = 8;
	nHealthMax = 8;
	fStateTick = 2.0f;

	pEquipedWeapon = (cWeapon*)Assets::get().GetItem("Covid Ball");
}

void cDynamic_Creature_Covid_Medium::Behavior(float fElapsedTime, cDynamic* player)
{
	// Check if player is nearby
	float fTargetX = player->px - px;
	float fTargetY = player->py - py;
	float fDistance = sqrtf(fTargetX * fTargetX + fTargetY * fTargetY);


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
				vy = -8.0f; // Jump toward player
				vx = fFaceDir * 2.0f;
				bAttackTick = true;
				bObjectOnGround = false; // Without this, would attack as it jumps AND when it lands. We just want to attack when lands. This bool wasn't getting set fast enough
			}
		}
		else
		{
			vx = 0;
			vy = 0;
		}

		fStateTick += 1.8f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.5f - 1.8f))); //Jump at random times between 1.8 and 2.5 seconds
	}

	//vx += fFaceDir * 1.0f * fElapsedTime;

	if (bObjectOnGround && bAttackTick && fDistance < 5.0f)
	{
		PerformAttack();
		vx = 0.0f;
		bAttackTick = false;
	}
}

void cDynamic_Creature_Covid_Medium::PerformAttack()
{
	if (pEquipedWeapon == nullptr)
		return;

	pEquipedWeapon->OnUse(this);
}