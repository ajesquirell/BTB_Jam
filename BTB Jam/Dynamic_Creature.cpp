#include "Dynamics.h"
#include "Assets.h"

cDynamic_Creature::cDynamic_Creature(std::string n) : cDynamic(n)
{
	nHealth = 10;
	nHealthMax = 10;
	fFaceDir = +1.0f;
	bSquat = false;
	fStateTick = 2.0f;
	bIsAttackable = true;

	// Base/ Default art for creatures - This should be changed for each derived creature!
	animations.mapStates["absent"].push_back(Assets::get().GetSprite("Absent"));
	animations.ChangeState("absent");
}

void cDynamic_Creature::DrawSelf(olc::PixelGameEngine* pge, float ox, float oy) //Screen space offset, since we already have position as data member
{
	//olc::GFX2D::Transform2D t;
	//t.Translate(-22 / 2, -22 / 2); //Align sprite to 0,0 to do affine transformations
	//t.Scale(fFaceDir, 1.0f); //BUG WITH THIS??? CUTS OFF A RIGHT COLUMN OF PIXELS WHEN REFLECTED? Yeah bug is in the PGEX/Scaling transformation somewhere. Could just double the png's used and switch like that instead of scaling

	////t.Rotate(fPlayerPosX);
	//t.Translate((px - ox) * 22 + 22/2, (py - oy) * 22 + 22/2); //If we want to change resolution later, need to make a global constant instead of 22

	//animations.DrawSelf(pge, t);

	if (fFaceDir == cDynamic_Creature::LEFT)
	{
		utility::DrawInvertedSprite(pge, ((px - ox) * 22) - GetDimensionDif(), ((py - oy) * 22) - GetDimensionDif(), animations.mapStates[animations.sCurrentState][animations.nCurrentFrame]);

		if (fDamageBlinkTimer > 0.0f)
		{
			for (int32_t i = animations.mapStates[animations.sCurrentState][animations.nCurrentFrame]->width; i > 0; i--)
				for (int32_t j = 0; j < animations.mapStates[animations.sCurrentState][animations.nCurrentFrame]->height; j++)
				{
					olc::Pixel p = animations.mapStates[animations.sCurrentState][animations.nCurrentFrame]->GetPixel(i, j);
					p.r = 180;
					p.g = 0;
					p.b = 0;

					pge->SetPixelMode(olc::Pixel::ALPHA);
					pge->SetPixelBlend(0.6);
					//pge->Draw(((px - ox) * 22) - GetDimensionDif() + i, ((py - oy) * 22) - GetDimensionDif() + j, p);
					pge->Draw(GetDimension() - i, ((py - oy) * 22) - GetDimensionDif() + j, p);
					pge->SetPixelMode(olc::Pixel::MASK);
				}
		}
	}
	else
	{
		pge->DrawSprite(((px - ox) * 22) - GetDimensionDif(), ((py - oy) * 22) - GetDimensionDif(), animations.mapStates[animations.sCurrentState][animations.nCurrentFrame]);

		if (fDamageBlinkTimer > 0.0f)
		{
			for (int32_t i = 0; i < animations.mapStates[animations.sCurrentState][animations.nCurrentFrame]->width; i++)
				for (int32_t j = 0; j < animations.mapStates[animations.sCurrentState][animations.nCurrentFrame]->height; j++)
				{
					olc::Pixel p = animations.mapStates[animations.sCurrentState][animations.nCurrentFrame]->GetPixel(i, j);
					p.r = 180;
					p.g = 0;
					p.b = 0;

					pge->SetPixelMode(olc::Pixel::ALPHA);
					pge->SetPixelBlend(0.6);
					pge->Draw(((px - ox) * 22) - GetDimensionDif() + i, ((py - oy) * 22) - GetDimensionDif() + j, p);
					pge->SetPixelMode(olc::Pixel::MASK);
				}
		}
	}
}

void cDynamic_Creature::Update(float fElapsedTime, cDynamic* player)
{
	if (fKnockBackTimer > 0.0f)
	{
		if (KnockBackMode == KnockBackMode::both_omnidirectional) // Use this if projectile/ Enemy is stationary
		{
			vx = fKnockBackDX * fKnockBackVel;
			vy = fKnockBackDY * fKnockBackVel;
		}
		else if (KnockBackMode == KnockBackMode::both) // Useful for projectiles you want to provide x and y direction knockback (x knockback is always in direction of projectile movement)
		{
			vx = fKnockBackVel * fKnockBackXDir; // Will always knock back the direction the projectile is facing/ moving
			vy = fKnockBackDY * fKnockBackVel; // Some Y direction factored in as well
		}
		else if (KnockBackMode == KnockBackMode::x_only) // Knock back in X direction only
		{
			vx = fKnockBackVel * fKnockBackXDir; // Will always knock back the direction the projectile is facing/ moving
		}
		else if (KnockBackMode == KnockBackMode::y_only) // Knock back in Y direction only
		{
			vy = fKnockBackDY * fKnockBackVel;
		}

		/*vx = fKnockBackDX * fKnockBackVel;
		vy = fKnockBackDY * fKnockBackVel;*/

		fKnockBackTimer -= fElapsedTime;
		if (fKnockBackTimer <= 0.0f)
		{
			fStateTick = 0.0f;
			bControllable = true;
			bIsAttackable = true;
			bSolidVsDynamic = true;
		}
	}
	else
	{
		UpdateAnimationState(fElapsedTime);

		// Update facing direction when under control of script processor. Otherwise handled by input (player) or in creature's behavior
		if (!g_script->bPlayerControlEnabled)
			fFaceDir = (vx < 0 ? -1.0f : vx > 0 ? 1.0f : fFaceDir);


		animations.Update(fElapsedTime); //Update animation frame

		if (bControllable)
			Behavior(fElapsedTime, player);
	}

	/*if (nHealth <= 0)
	{
		animations.ChangeState("dead");
		bControllable = false;
		bSolidVsDynamic = false;
		bSolidVsMap = false;
		OnDead();
	}*/

	fDamageBlinkTimer -= fElapsedTime;
}

void cDynamic_Creature::KnockBack(float dx, float dy, cDynamic_Projectile* proj)
{
	KnockBackMode = proj->KnockBackMode;
	fKnockBackDX = dx;
	fKnockBackDY = dy;
	fKnockBackVel = proj->fKnockBackVel;
	fKnockBackXDir = proj->fFaceDir;
	fKnockBackTimer = proj->fKnockBackDuration;
	bSolidVsDynamic = false;
	bControllable = false;
	bIsAttackable = false;

	fDamageBlinkTimer = 0.4f;
}

void cDynamic_Creature::Behavior(float fElapsedTime, cDynamic* player)
{
	//No default behavior
}

void cDynamic_Creature::UpdateAnimationState(float fElapsedTime)
{
}
