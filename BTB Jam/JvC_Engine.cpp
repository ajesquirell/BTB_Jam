﻿#include "JvC_Engine.h"

#define CMD(n) m_script.AddCommand(new cCommand_ ## n)

Platformer::Platformer()
{
	sAppName = "BTB Jam";
}

bool Platformer::OnUserCreate()
{
	//Only instance of spaghetti code
	//But allows us to easily access all the
	//public utilities from this file - our game engine
	cCommand::g_engine = this;
	cQuest::g_engine = this;
	cDynamic::g_engine = this;
	cItem::g_engine = this;

	cMap::g_script = &m_script;
	cQuest::g_script = &m_script;
	cDynamic::g_script = &m_script;

	//Initialize "back buffer" - really just for inventory background
	backBuff = new olc::Sprite(ScreenWidth(), ScreenHeight());

	//Inventory background color
	inventoryColor = olc::BLACK;
	rectColor = olc::WHITE;

	//Load Assets (Sprites, Maps)
	Assets::get().LoadSprites(); //Can get away with loading everything at once because this is a small game
	Assets::get().LoadItems();
	Assets::get().LoadMaps();
	Assets::get().LoadSounds();

	//Animated

	SetPixelMode(olc::Pixel::MASK); //Allow Transparency


	//Sound
	olc::SOUND::InitialiseAudio(44100U, 1U, 8U, 512U);

	//Add First Quest
	listQuests.push_front(new cQuest_MainQuest());

	//Player init
	m_pPlayer = new cDynamic_Creature_Javid(); //For now sprites/ anims are hard coded to be Javid
	GiveItem(m_pPlayer->pEquipedWeapon);

	//Initial Map
	ChangeMap("Coronatower", 1, Assets::get().GetMap("Coronatower")->nHeight - 2);
	fCameraPosY = pCurrentMap->nHeight;

	return true;
}

bool Platformer::OnUserDestroy()
{
	olc::SOUND::DestroyAudio();
	return true;
}

bool Platformer::OnUserUpdate(float fElapsedTime)
{
	//Check for game pause
	if (GetKey(olc::Key::ESCAPE).bPressed)
	{
		if (!bGamePaused) {
			bGamePaused = true; /*Pause*/
			olc::SOUND::StopSample(Assets::get().GetSound("CleanSlate"));
			//olc::SOUND::PlaySample(Assets::get().GetSound("sndGetMoney"));
		}
		else {
			bGamePaused = false; /*Unpause*/
			olc::SOUND::PlaySample(Assets::get().GetSound("CleanSlate"), true);
		}
	}
	if (bGamePaused)
	{
		DrawString(ScreenWidth() / 2.5f, ScreenHeight() / 2.5f, "Paused");
		return true; //Game loop won't execute
	}

	//Check Game Mode
	switch (nGameMode)
	{
	case MODE_TITLE:
		return UpdateTitleScreen(fElapsedTime);
	case MODE_LOCAL_MAP:
		return UpdateLocalMap(fElapsedTime);
	//case MODE_WORLD_MAP:
		//return UpdateWorldMap(fElapsedTime);
	case MODE_INVENTORY:
		return UpdateInventory(fElapsedTime);
	//case MODE_SHOP:
		//return UpdateShop(fElapsedTime);
	case MODE_GAMEOVER:
		return UpdateGameOver(fElapsedTime);
	}

	return true;
}

bool Platformer::UpdateTitleScreen(float fElapsedTime)
{
	FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);

	//Update script
	m_script.ProcessCommands(fElapsedTime);

	

	fTitleStateTick -= fElapsedTime;
	if (fTitleStateTick <= 0.0f)
	{
			nSequenceCnt++; // Increase sequence number

		// Sequence number has already been incremented, so this adds time to the new current sequence
		if (nSequenceCnt == 1)
		{
			fTitleStateTick = 1.0f;
		}
		if (nSequenceCnt == 2)
		{
			fTitleStateTick = 2.0f;
		}
		if (nSequenceCnt == 3)
		{
			olc::SOUND::PlaySample(Assets::get().GetSound("CleanSlate"), true); // Plays loop
			CMD(ShowDialog({ "Hello World!", "Covid-19 is worse than we ever imagined" }));
		}
		if (nSequenceCnt == 4)
		{
			//m_script.CompleteCommand();
			CMD(ShowDialog({ "We are all going to die!"}));
			CMD(ShowDialog({ "Unless you can stop covid", "Blah Blah Blah" }));
			fTitleStateTick = 3.0f;
		}
	}

	if (nSequenceCnt == 0 || nSequenceCnt == 2) // This executes while the time is running out
	{
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(rand() % 255, rand() % 255, rand() % 255));
	}
	if (nSequenceCnt >= 3)
	{
		DisplayDialog({ "Press X to continue" }, 20, 70);
	}



	// Draw any dialog being displayed
	if (bShowDialog)
		DisplayDialog(vecDialogToShow, 10, 40);

	if (bShowDialog)
	{
		if (GetKey(olc::Key::X).bPressed)
		{
			bShowDialog = false;
			m_script.CompleteCommand();
			//nSequenceCnt++;
		}
	}


	DrawString(ScreenWidth() / 3, ScreenHeight() - 20, "Press SPACE to skip...");
	if (GetKey(olc::Key::SPACE).bPressed)
	{
		if (olc::SOUND::listActiveSamples.empty())
			olc::SOUND::PlaySample(Assets::get().GetSound("CleanSlate"), true); // Plays loop

		nGameMode = MODE_LOCAL_MAP;
	}


	return true;
}

bool Platformer::UpdateLocalMap(float fElapsedTime)
{

	//Update script
	m_script.ProcessCommands(fElapsedTime);

	// Erase and delete redundant projectiles
	vecProjectiles.erase(
		remove_if(vecProjectiles.begin(), vecProjectiles.end(),
			[](const cDynamic* d) {return ((cDynamic_Projectile*)d)->bRedundant; }), vecProjectiles.end());

	if (m_script.bPlayerControlEnabled) // Commands/ script processor allow player control
	{
		if (m_pPlayer->bControllable) // Knock Back allows player control
		{
			//Handle Input
			if (IsFocused())
			{
				if (GetKey(olc::Key::UP).bHeld)
				{
					m_pPlayer->vy = -6.0f;
				}

				m_pPlayer->bSquat = false; //Reset flag
				if (GetKey(olc::Key::DOWN).bHeld)
				{
					//m_pPlayer->vy = 6.0f;
					m_pPlayer->bSquat = true;
				}

				if (GetKey(olc::Key::LEFT).bHeld && !GetKey(olc::Key::RIGHT).bHeld) //LEFT (and ONLY left - otherwise b/c of my velocity mechanics you can accelerate while in "braking" positon if you hold down both buttons
				{
					if (!GetKey(olc::Key::DOWN).bHeld) //Stop movement if crouching/squatting
						m_pPlayer->vx += (m_pPlayer->bObjectOnGround && m_pPlayer->vx <= 0 ? -25.0f : m_pPlayer->bObjectOnGround ? -8.0f : -14.0f) * fElapsedTime; //If on ground accelerating / else if on ground braking+turning around/ else in air
																																		//Player has more control on ground rather than in air, and when turning around it goes a little slower, feels more like og Mario			
					m_pPlayer->fFaceDir = -1.0f; //When drawing, we will scale player with this to give him correct facing							//14.0f is perfect in-air number - when running and jumping you can't quite make it back to the same block you started on
					//fFaceDir = bPlayerOnGround ? -1.0f : fFaceDir; //More like original NES Mario - can only change direction when on ground
				}

				if (GetKey(olc::Key::RIGHT).bHeld && !GetKey(olc::Key::LEFT).bHeld) //RIGHT
				{
					if (!GetKey(olc::Key::DOWN).bHeld) //Stop movement if crouching/squatting
						m_pPlayer->vx += (m_pPlayer->bObjectOnGround && m_pPlayer->vx >= 0 ? 25.0f : m_pPlayer->bObjectOnGround ? 8.0f : 14.0f) * fElapsedTime;

					m_pPlayer->fFaceDir = +1.0f;
					//fFaceDir = bPlayerOnGround ? +1.0f : fFaceDir;
				}

				if (GetKey(olc::Key::Z).bReleased) //Access Inventory
				{
					inventoryColor.a = 0; //Reset alpha to do fading
					nInvSelectX = 0;
					nInvSelectY = 0;
					vecInvSelect.clear();
					vecInvSelect.push_back(new InvSelect(nInvSelectX, nInvSelectY, 255));
					nGameMode = MODE_INVENTORY;
				}

				if (GetKey(olc::Key::SPACE).bPressed)
				{
					if (m_pPlayer->bObjectOnGround)
					{
						m_pPlayer->vy = -12.0f;
						//olc::SOUND::PlaySample(sndSampleA); // Plays Sample A
						olc::SOUND::PlaySample(Assets::get().GetSound("sndBoo"));
					}
				}
				if (!GetKey(olc::Key::SPACE).bHeld) //Allows for variable jump height depending on how long space is pressed
				{
					if (m_pPlayer->vy < 0) //Is currently jumping
					{
						m_pPlayer->vy += 35.0f * fElapsedTime;
					}
				}

				if (GetKey(olc::Key::A).bPressed) //TEST/DEBUG
				{
					//CMD(MoveTo(m_pPlayer, 0, 9, 1.0f));
					//CMD(MoveTo(vecDynamics[1], 1, 9, 2.0f));
					//CMD(MoveTo(vecDynamics[2], 1, 9, 2.0f));
					//CMD(ShowDialog({ "Oh silly Javid" }));
					//CMD(ShowDialog({ "I think OOP", "is really useful" }, olc::RED));
					//CMD(MoveTo(m_pPlayer, 7, 9, 1.0f));
					//CMD(ChangeMap("Level 2", 0.0f, 0.0f));
					CMD(PanCamera(!bPanCamera));
				}

				if (GetKey(olc::Key::X).bPressed) // Interaction
				{
					// Grab a point from the direction the player is facing and check for interactions
					float fTestX, fTestY;

					if (m_pPlayer->fFaceDir == 1.0f)
					{
						fTestX = m_pPlayer->px + 1.5f; //Test probe in center of adjacent tile
						fTestY = m_pPlayer->py + 0.5f;
					}
					if (m_pPlayer->fFaceDir == -1.0f)
					{
						fTestX = m_pPlayer->px - 0.5f;
						fTestY = m_pPlayer->py + 0.5f;
					}

					// Check if test point has hit a dynamic object
					bool bInteraction = false;
					for (auto dyns : vecDynamics)
					{
						if (fTestX > dyns->px && fTestX < (dyns->px + 1.0f)
							&& fTestY > dyns->py && fTestY < (dyns->py + 1.0f))
						{
							if (dyns->bFriendly)
							{
								// Iterate through stack until something responds
								// --Base quest should capture interactions not specified by other quests

								// First check if it's quest related
								for (auto& quest : listQuests)
									if (quest->OnInteraction(vecDynamics, dyns, cQuest::TALK))
									{
										bInteraction = true;
										break;
									}

								if (!bInteraction)
								{
									// Some objects just do stuff when you interact with them
									if (dyns->OnInteract(m_pPlayer)) //ADD TALK NATURE
										bInteraction = true;
								}

								if (!bInteraction)
								{
									// Then check if it's map related
									if (pCurrentMap->OnInteraction(vecDynamics, dyns, cMap::TALK))
										bInteraction = true;
								}
							}
							else
							{
								// Interaction not friendly - only enemies
								// are not friendly - so attack
								m_pPlayer->PerformAttack();
							}
						}
					}

					if (!bInteraction) // Default action is attack
					{
						m_pPlayer->PerformAttack();
					}
				}
			}
		}
	}
	else // Script processor has control 
	{
		// Simply adjust facing direction here.Other objects will do this in behavior --HANDLED IN CREATURE CLASS NOW FOR ALL CREATURES UNDER CONTROL OF SCRIPT
		//m_pPlayer->fFaceDir = (m_pPlayer->vx < 0 ? -1.0f : m_pPlayer->vx > 0 ? 1.0f : m_pPlayer->fFaceDir);

		if (bShowDialog)
		{
			if (GetKey(olc::Key::X).bPressed)
			{
				bShowDialog = false;
				m_script.CompleteCommand();
			}
		}
	}



	//Loop through dynamic objects
	bool bWorkingWithProjectiles = false;
	for (auto& source : { &vecDynamics, &vecProjectiles }) // Glue together vecDynamics and vecProjectiles to reuse code,
															// but the two vectors don't interfere with each other
	{																
		for (auto& object : *source)
		{
			if (object->bGravityApplies)
				object->vy += 20.0f * fElapsedTime; //Gravity

			if (object->bObjectOnGround)
			{
				object->vx += -3.0f * object->vx * fElapsedTime; //Add some drag so it doesn't feel like ice
				if (fabs(object->vx) < 0.05f) //Clamp vel to 0 if near 0 to allow player to stop
				{
					if (object == m_pPlayer) {
						if (!GetKey(olc::Key::RIGHT).bHeld && !GetKey(olc::Key::LEFT).bHeld) //In release mode, fps is so high that because of fElapsedTime scaling acceleration																		//it wouldn't be able to get past this stopping threshold, leaving player unable to move - if statement is soln
							object->vx = 0.0f;
					}
					//else object not player, and method to stop
					//should be defined in behavior
				}
			}


			////Animation overrides
			//if (bSquat)
			//	animPlayer.ChangeState("squat");

			//Clamp Velocity to prevent getting out of control
			if (!object->bIsProjectile)
			{
				if (object->vx > 10.0f)
					object->vx = 10.0f;

				if (object->vx < -10.0f)
					object->vx = -10.0f;

				if (object->vy > 100.0f)
					object->vy = 100.0f;

				if (object->vy < -100.0f)
					object->vy = -100.0f;
			}

			//Calculate potential new position
			float fNewObjectPosX = object->px + object->vx * fElapsedTime;
			float fNewObjectPosY = object->py + object->vy * fElapsedTime;


			//Collision (Object vs Map)
			if (object->bSolidVsMap)
			{
				bool bCollisionWithMap = false; //For making projectiles "redundant" when hit map

				if (object->vx <= 0) //Player moving left
				{
					if (pCurrentMap->GetTile(fNewObjectPosX + 0.0f - (object->GetDimensionDif() / 22.0f), object->py + 0.0f - (object->GetDimensionDif() / 22.0f))->solid || pCurrentMap->GetTile(fNewObjectPosX + 0.0f - (object->GetDimensionDif() / 22.0f), object->py + 0.9f)->solid)  //0.9f because we're not checking Y direction collision right here, and we don't want that to register a collsion, but we still have to check that bottom left corner of the player
					{																																																												//And the 0.9f allows player to fit in gaps that are only 1 unit across
						if (object->GetDimensionDif() == 0) // Standard size																																																			//Basically makes so truncation of tiles doesn't catch us.
							fNewObjectPosX = (int)fNewObjectPosX + 1;		
						else // Non-standard size object
							fNewObjectPosX = (int)fNewObjectPosX + (object->GetDimensionDif() / 22.0f);

						object->vx = 0;
						bCollisionWithMap = true;
					}
				}
				else if (object->vx > 0) //Player moving Right
				{
					if (pCurrentMap->GetTile(fNewObjectPosX + 1.0f, object->py + 0.0f - (object->GetDimensionDif() / 22.0f))->solid || pCurrentMap->GetTile(fNewObjectPosX + 1.0f, object->py + 0.9f)->solid)
					{
						fNewObjectPosX = (int)fNewObjectPosX;
						object->vx = 0;
						bCollisionWithMap = true;
					}

				}

				if (object->bIsProjectile && bCollisionWithMap)
				{
					object->bRedundant = true;
				}

				object->bObjectOnGround = false; //Clear flag

				if (object->vy <= 0) //Player moving up
				{
					//Already resolved X-direction collisions, so we can use the new X position and new Y position
					if (pCurrentMap->GetTile(fNewObjectPosX + 0.0f - (object->GetDimensionDif() / 22.0f), fNewObjectPosY + 0.0f - (object->GetDimensionDif() / 22.0f))->solid || pCurrentMap->GetTile(fNewObjectPosX + 0.99999f, fNewObjectPosY + 0.0f - (object->GetDimensionDif() / 22.0f))->solid)
					{
						/***Check for breakable blocks (putting here allows for collision AND breaking)***/  // Breaking/ punching not supported for non-standard sprite sizes (yet?) due to time, and ideally for now only player will be breaking blocks anyways
						if (pCurrentMap->GetTile(fNewObjectPosX + 0.0f, fNewObjectPosY + 0.0f)->solid && pCurrentMap->GetTile(fNewObjectPosX + 1.0f, fNewObjectPosY + 0.0f)->solid) //Needs to be first in if statement(checked first)
						{
							pCurrentMap->GetTile(fNewObjectPosX + 0.5f, fNewObjectPosY + 0.0f)->OnPunch(); //Only break one block at a time
						}

						else if (pCurrentMap->GetTile(fNewObjectPosX + 0.0f, fNewObjectPosY + 0.0f)->solid)
						{
							pCurrentMap->GetTile(fNewObjectPosX + 0.0f, fNewObjectPosY + 0.0f)->OnPunch();
						}

						else if (pCurrentMap->GetTile(fNewObjectPosX + 1.0f, fNewObjectPosY + 0.0f)->solid)
						{
							pCurrentMap->GetTile(fNewObjectPosX + 1.0f, fNewObjectPosY + 0.0f)->OnPunch();
						}

						/***********************************************************************************/

						if (object->GetDimensionDif() == 0) // Standard Size
							fNewObjectPosY = (int)fNewObjectPosY + 1;
						else
							fNewObjectPosY = (int)fNewObjectPosY + (object->GetDimensionDif() / 22.0f);

						object->vy = 0;
					}
				}
				else //Player moving down
				{
					//Allow to fit in 1 wide gap going down
					if (pCurrentMap->GetTile(fNewObjectPosX + 0.0f + 0.15f - (object->GetDimensionDif() / 22.0f), fNewObjectPosY + 1.0f)->solid || pCurrentMap->GetTile(fNewObjectPosX + 0.99999f - 0.15f, fNewObjectPosY + 1.0f)->solid)
					{
						fNewObjectPosY = (int)fNewObjectPosY;
						object->vy = 0;
						object->bObjectOnGround = true;
					}
					//Still allow to snap into 1 wide opening in wall going left or right
					else if ((GetKey(olc::Key::LEFT).bHeld || GetKey(olc::Key::RIGHT).bHeld) && !GetKey(olc::Key::DOWN).bHeld) //Change cause doesn't apply to npc's
					{
						if (pCurrentMap->GetTile(fNewObjectPosX + 0.0f, fNewObjectPosY + 1.0f)->solid || pCurrentMap->GetTile(fNewObjectPosX + 0.99999f, fNewObjectPosY + 1.0f)->solid)
						{
							fNewObjectPosY = (int)fNewObjectPosY;
							object->vy = 0;
							object->bObjectOnGround = true;
						}
					}
				}
			}


			float fDynamicObjectPosX = fNewObjectPosX;
			float fDynamicObjectPosY = fNewObjectPosY;

			// Dynamic Object Collision (Object vs Object)
			for (auto& dyn : vecDynamics) // Loop through all OTHER dynamic objects
			{
				if (dyn != object) // Don't check object against itself
				{
					// If objects are solid then they must not overlap
					if (dyn->bSolidVsDynamic && object->bSolidVsDynamic)
					{
						if (fDynamicObjectPosX - (object->GetDimensionDif() / 22.0f) < (dyn->px + 1.0f) && (fDynamicObjectPosX + 1.0f) > dyn->px - (dyn->GetDimensionDif() / 22.0f)
							&& object->py - (object->GetDimensionDif() / 22.0f) < (dyn->py + 1.0f) && (object->py + 1.0f) > dyn->py - (dyn->GetDimensionDif() / 22.0f))
						{
							// First check horizontally - Left first
							if (object->vx <= 0)
								fDynamicObjectPosX = dyn->px + 1.0f + (object->GetDimensionDif() / 22.0f);
							else
								fDynamicObjectPosX = dyn->px - 1.0f - (dyn->GetDimensionDif() / 22.0f);

							object->vx = 0;

							if (object == m_pPlayer) // If player runs in to enemy
							{
								dyn->OnInteract(object); // For touching enemies - OnInteract will hurt player
								/*olc::Pixel p(rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1);
								pCurrentMap->skyColor = p;*/
							}
							if (dyn == m_pPlayer) // If enemy runs into player (there is a difference)
							{
								object->OnInteract(dyn); // For touching enemies - OnInteract will hurt player
								/*olc::Pixel p(rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1);
								pCurrentMap->skyColor = p;*/
							}
						}

						if (fDynamicObjectPosX  - (object->GetDimensionDif() / 22.0f) < (dyn->px + 1.0f) && (fDynamicObjectPosX + 1.0f) > dyn->px - (dyn->GetDimensionDif() / 22.0f)
							&& fDynamicObjectPosY - (object->GetDimensionDif() / 22.0f) < (dyn->py + 1.0f) && (fDynamicObjectPosY + 1.0f) > dyn->py - (dyn->GetDimensionDif() / 22.0f))
						{
							bool bPlayerBounce = false;
							bool bEnemyBounce = false;
							//Then check vertically - Up first
							if (object->vy <= 0)
								fDynamicObjectPosY = dyn->py + 1.0f + (object->GetDimensionDif() / 22.0f);
							else
							{
								fDynamicObjectPosY = dyn->py - 1.0f - (dyn->GetDimensionDif() / 22.0f);
								if (object == m_pPlayer) { bPlayerBounce = true; } // If player lands on top of enemy
								if (dyn == m_pPlayer) { bEnemyBounce = true; } // If enemy lands on top of player
								object->bObjectOnGround = true;
							}

							if (bPlayerBounce)
								object->vy = -12.0f;
							else if (bEnemyBounce)
							{
								object->vy = -6.0f;
								object->vx = object->px >= dyn->px ? 4.0f : -4.0f;
							}
							else
								object->vy = 0;

							if (dyn == m_pPlayer && bEnemyBounce == true) // Only want player to be damaged if object lands on top of player. Player should bounce off enemy heads 
							{
								object->OnInteract(dyn); // For touching enemies - OnInteract will hurt player
							}
						}

					}
					else
					{
						if (object == m_pPlayer) // Only player can interact with items and such (could change to make things interesting)
						{
							bool bInteraction = false; //Not sure if going to use - would use this same way as with input, so we limit the number of responses we get from an interaction to only the most important
							if (fDynamicObjectPosX - (object->GetDimensionDif() / 22.0f) < (dyn->px + 1.0f) && (fDynamicObjectPosX + 1.0f) > dyn->px - (dyn->GetDimensionDif() / 22.0f)
								&& fDynamicObjectPosY - (object->GetDimensionDif() / 22.0f) < (dyn->py + 1.0f) && (fDynamicObjectPosY + 1.0f) > dyn->py - (dyn->GetDimensionDif() / 22.0f))
							{
								if (!bInteraction)
								{
									// Check if interaction is map related
									pCurrentMap->OnInteraction(vecDynamics, dyn, cMap::WALK);
									//bInteraction = true; //Do we need this?
								}

								if (!bInteraction)
								{
									//Finally just check the object - (for items, non-important characters, etc)
									dyn->OnInteract(object);
									//bInteraction = true; //Do we need this?
								}
							}
						}
						else
						{
							if (bWorkingWithProjectiles)
							{
								if (fDynamicObjectPosX - (object->GetDimensionDif() / 22.0f) < (dyn->px + 1.0f) && (fDynamicObjectPosX + 1.0f) > dyn->px - (dyn->GetDimensionDif() / 22.0f)
									&& fDynamicObjectPosY - (object->GetDimensionDif() / 22.0f) < (dyn->py + 1.0f) && (fDynamicObjectPosY + 1.0f) > dyn->py - (dyn->GetDimensionDif() / 22.0f))
								{
									if (dyn->bFriendly != object->bFriendly)
									{
										// We know object is a projectile, so dyn is something
										// opposite that it has overlapped with
										if (dyn->bIsAttackable)
										{
											// Dynamic object is a creature
											Damage((cDynamic_Projectile*)object, (cDynamic_Creature*)dyn);
										}
									}
								}
							}
						}
					}
				}
			}




			object->px = fDynamicObjectPosX;
			object->py = fDynamicObjectPosY;


			//Update dynamic objects
			object->Update(fElapsedTime, m_pPlayer);
		}
		bWorkingWithProjectiles = true; // Will repeat all this code for vecProjectiles
	}
		
	// Remove quests that have been completed
	auto i = remove_if(listQuests.begin(), listQuests.end(), [](const cQuest* d) {return d->bCompleted; });
	if (i != listQuests.end())
		listQuests.erase(i);
	

	//Update Tiles - for tile animations & other calculations
	for (int x = 0; x < pCurrentMap->nWidth; x++)
	{
		for (int y = 0; y < pCurrentMap->nHeight; y++)
		{
			pCurrentMap->GetTile(x, y)->Update(fElapsedTime, m_pPlayer);
		}
	}


	fCameraPosX = m_pPlayer->px;
	if (bPanCamera)
		fCameraPosY += -1.0f * fElapsedTime;

	//Draw Level 
	int nTileWidth = 22;
	int nTileHeight = 22;
	int nVisibleTilesX = ScreenWidth() / nTileWidth;
	int nVisibleTilesY = ScreenHeight() / nTileHeight;

	//Calculate Top-Leftmost visible tile
	float fOffsetX = fCameraPosX - (float)nVisibleTilesX / 2.0f;
	float fOffsetY = fCameraPosY - (float)nVisibleTilesY / 2.0f;

	//Clamp camera to game boundaries
	if (fOffsetX < 0) fOffsetX = 0;
	if (fOffsetY < 0) fOffsetY = 0;
	if (fOffsetX > pCurrentMap->nWidth - nVisibleTilesX) fOffsetX = pCurrentMap->nWidth - nVisibleTilesX;
	if (fOffsetY > pCurrentMap->nHeight - nVisibleTilesY) fOffsetY = pCurrentMap->nHeight - nVisibleTilesY;

	// Get offsets for smooth movement
	float fTileOffsetX = (fOffsetX - (int)fOffsetX) * nTileWidth;
	float fTileOffsetY = (fOffsetY - (int)fOffsetY) * nTileHeight;

	SetDrawTarget(backBuff);

	//Draw visible tile map (overdraw to prevent weird artifacts at screen edges)
	for (int x = -1; x < nVisibleTilesX + 1; x++)
	{
		for (int y = -1; y < nVisibleTilesY + 1; y++)
		{
			FillRect(x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY, nTileWidth, nTileHeight, pCurrentMap->skyColor); //Background fill -- "sky"
			pCurrentMap->GetTile(x + fOffsetX, y + fOffsetY)->DrawSelf(this, x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY);
		}
	}

	//Make sky darker as we climb higher
	olc::Pixel p = pCurrentMap->skyColor;
	p.r = ((pCurrentMap->nHeight - m_pPlayer->py) / pCurrentMap->nHeight) * 100;
	p.g = (m_pPlayer->py / pCurrentMap->nHeight) * 255;
	p.b = (m_pPlayer->py / pCurrentMap->nHeight) * 255;
	p.a = 255;
	pCurrentMap->skyColor = p;
	

	// Draw Object
	for (auto& source : { &vecDynamics, &vecProjectiles })
		for (auto& dyns : *source)
			dyns->DrawSelf(this, fOffsetX, fOffsetY);

	m_pPlayer->DrawSelf(this, fOffsetX, fOffsetY); // May be a bit wasteful, and could just iterate backwards through ranged for loop above so that player is drawn last... figure out later

	SetDrawTarget(nullptr);
	DrawSprite(0, 0, backBuff);

	// Draw Score
	sScoreString = "Toilet Paper: " + to_string(m_pPlayer->nScore);
	DrawString(1, 1, sScoreString, olc::RED);

	// Draw Health
	DrawString(1, 11, "HP: ", olc::DARK_BLUE);
	for (int x = 0; x < m_pPlayer->nHealth; x++)
		FillRect(25 + (x * 6), 10, 5, 8, olc::DARK_BLUE);
	for (int x = 0; x < m_pPlayer->nHealthMax; x++)
		DrawRect(25 + (x * 6), 10, 4, 7, olc::DARK_BLUE);

	// Draw any dialog being displayed
	if (bShowDialog)
		DisplayDialog(vecDialogToShow, 20, 20);


	/*
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Debug+Testing
	if (GetKey(olc::Key::D).bPressed)
		if (showDebug)
			showDebug = false;
		else
			showDebug = true;

	if (showDebug)
	{
		DrawString(0, 15, "Debug: (D to hide)");
		//DrawString(0, 24, "Time Between Animation: " + to_string(animPlayer.fTimeBetweenFrames));
		DrawString(1, 33, "X-Velocity: " + to_string(m_pPlayer->vx) + "\nY-Velocity: " + to_string(m_pPlayer->vy));
		DrawString(1, 53, to_string(fElapsedTime));
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	*/

	DrawString(140, 1, "Javid vs. Covid", olc::Pixel(rand() % 255, rand() % 255, rand() % 255));
	DrawString(0, ScreenHeight() - 25, "MOVE: <- ->, JUMP: Space\nInteract: X, Inventory: Z\nPAUSE: Esc", olc::DARK_MAGENTA);

	//Game end
	if (m_pPlayer->nHealth <= 0)
	{
		//nGameMode = MODE_GAMEOVER;
	}


	return true;
}

void Platformer::ShowDialog(std::vector<std::string> vecLines, olc::Pixel color)
{
	vecDialogToShow = vecLines;
	bShowDialog = true;
	dialogColor = color;
}


void Platformer::DisplayDialog(vector<string> vecLines, int x, int y)
{
	// Display dialog in a nice looking way
	int nMaxLineLength = 0;
	int nLines = vecLines.size();

	for (auto l : vecLines) if (l.size() > nMaxLineLength) nMaxLineLength = l.size();

	// Draw Box (Default Font size is 8x8, so that constant is used here)
	FillRect(x - 1, y - 1, nMaxLineLength * 8 + 2, nLines * 8 + 2, dialogColor);
	DrawRect(x - 2, y - 2, nMaxLineLength * 8 + 3, nLines * 8 + 3);

	for (int l = 0; l < vecLines.size(); l++)
	{
		DrawString(x, y + l * 8, vecLines[l], olc::WHITE);
	}
}

void Platformer::ChangeMap(string sMapName, float x, float y)
{
	cMap* newMap = Assets::get().GetMap(sMapName);
	if (newMap != nullptr) //Won't work if name passed in is wrong
	{
		// Destroy all dynamics - LATER USE delete FOR OBJECTS ON HEAP
		vecDynamics.clear();
		vecDynamics.push_back(m_pPlayer);

		// Set current map
		pCurrentMap = newMap;

		// Update player location
		m_pPlayer->px = x;
		m_pPlayer->py = y;

		// Create new dynamics from map
		pCurrentMap->PopulateDynamics(vecDynamics);

		// Create new dynamics from quests
		for (auto q : listQuests)
			q->PopulateDynamics(vecDynamics, pCurrentMap->sName);
	}
}

void Platformer::AddQuest(cQuest* quest)
{
	listQuests.push_front(quest);
}

void Platformer::PanCamera(bool b)
{
	bPanCamera = b;
}

bool Platformer::GiveItem(cItem* item)
{
	//m_script.AddCommand(new cCommand_ShowDialog({ "You have found a" , item->sName }));
	listItems.push_back(item);
	return true;
}

bool Platformer::TakeItem(cItem* item)
{
	if (item != nullptr)
	{
		listItems.erase(find(listItems.begin(), listItems.end(), item));
		return true;
	}
	else
		return false;
}

bool Platformer::HasItem(cItem* item)
{
	if (item != nullptr)
	{
		return find(listItems.begin(), listItems.end(), item) != listItems.end();
	}
	else
		return false;
}

bool Platformer::UpdateInventory(float fElapsedTime)
{
	fInvStateTick += fElapsedTime;
	if (fInvStateTick >= 0.02f) //Draw things that use fading
	{
		if (inventoryColor.a + 35 < 255)
			inventoryColor.a += 35;
		else
			inventoryColor.a = 255;

		// ===== Drawing background and fade overlay NOT every frame because improves fps, and for an inventory you don't even notice =====
		//Draw Background
		DrawSprite(0, 0, backBuff);

		//Draw Fade-in Background overlay
		SetPixelMode(olc::Pixel::ALPHA);
		SetPixelBlend(0.7f);
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), inventoryColor);
		SetPixelBlend(1.0f);
		SetPixelMode(olc::Pixel::MASK);
		//==================================================================================================================================

		//Draw selection rectangle (Fade)
		for (int i = 0; i < vecInvSelect.size(); i++)
		{
			if (vecInvSelect[i]->x != nInvSelectX || vecInvSelect[i]->y != nInvSelectY)
			{
				//Fade
				if (vecInvSelect[i]->alpha - 15 > 0)
					vecInvSelect[i]->alpha -= 15;
				else
					vecInvSelect[i]->alpha = 0;
			}
		
			rectColor.a = vecInvSelect[i]->alpha;
			SetPixelMode(olc::Pixel::ALPHA);
			DrawRect(6 + vecInvSelect[i]->x * 25, 24 + vecInvSelect[i]->y * 25, 25, 25, rectColor);
			SetPixelMode(olc::Pixel::MASK);
		}

		fInvStateTick -= 0.02f;
	}

	DrawString(4, 4, "INVENTORY", olc::WHITE, 2);

	int i = 0;
	cItem* highlighted = nullptr;

	// Draw Consumables
	for (auto& item : listItems)
	{
		int x = i % 4;
		int y = i / 4;
		i++;

		olc::GFX2D::Transform2D t;
		t.Translate(8 + x * 25, 26 + y * 25);
		item->DrawSelf(this, t);

		if (nInvSelectX == x && nInvSelectY == y)
			highlighted = item;

		//Update dynamic objects in case items have animation
		item->Update(fElapsedTime);

	}

	if (GetKey(olc::LEFT).bPressed || GetKey(olc::RIGHT).bPressed || GetKey(olc::UP).bPressed || GetKey(olc::DOWN).bPressed)
	{
		if (GetKey(olc::LEFT).bPressed) nInvSelectX--;
		if (GetKey(olc::RIGHT).bPressed) nInvSelectX++;
		if (GetKey(olc::UP).bPressed) nInvSelectY--;
		if (GetKey(olc::DOWN).bPressed) nInvSelectY++;

		if (nInvSelectX < 0) nInvSelectX = 3;
		if (nInvSelectX >= 4) nInvSelectX = 0;
		if (nInvSelectY < 0) nInvSelectY = 3;
		if (nInvSelectY >= 4) nInvSelectY = 0;


		vecInvSelect.insert(vecInvSelect.begin(), new InvSelect(nInvSelectX, nInvSelectY, 255));
	}

	//Remove the selector rects that have faded away
	auto v = remove_if(vecInvSelect.begin(), vecInvSelect.end(), [](const InvSelect* element) {return element->alpha == 0; });
	if (v != vecInvSelect.end())
		vecInvSelect.erase(v);


	if (GetKey(olc::Key::Z).bReleased)
	{
		nGameMode = MODE_LOCAL_MAP;
	}

	if (highlighted != nullptr)
	{
		DrawString(8, 160, "SELECTED:");
		DrawString(8, 170, highlighted->sName);

		DrawString(8, 190, "DESCRIPTION:");
		DrawString(8, 200, highlighted->sDescription);

		if (!highlighted->bKeyItem)
		{
			DrawString(80, 160, "(Press X to use)");
		}

		if (GetKey(olc::Key::X).bPressed)
		{
			//Use selected item
			if (!highlighted->bKeyItem)
			{
				if (highlighted->OnUse(m_pPlayer))
				{
					// Item has signaled it has been (fully) used, so remove it
					TakeItem(highlighted);
				}
			}
			else
			{
				
			}
		}
	}

	DrawString(128, 20, "LOCATION:");
	DrawString(128, 28, pCurrentMap->sName);

	DrawString(128, 44, "HEALTH:" + to_string(m_pPlayer->nHealth));
	DrawString(128, 52, "MAX HEALTH:" + to_string(m_pPlayer->nHealthMax));

	return true;
}

void Platformer::Attack(cDynamic_Creature* aggressor, cWeapon* weapon)
{
	// Don't use weapon on victim, use on aggressor - could modify weapon's behavior based on user
	weapon->OnUse(aggressor);
}

void Platformer::AddProjectile(cDynamic_Projectile* proj)
{
	vecProjectiles.push_back(proj);
}

void Platformer::Damage(cDynamic_Projectile* projectile, cDynamic_Creature* victim)
{
	if (victim != nullptr)
	{
		// Attack victim with damage (OOP will allow each object to handle going below 0 in its own way)
		victim->nHealth -= projectile->nDamage;

		// Knock victim back
		float tx = victim->px - projectile->px;
		float ty = victim->py - projectile->py;

		float d = sqrtf(tx * tx + ty * ty);

		if (d < 0.01f) d = 0.1f; // This makes knock back have a lesser effect if the enemy is closer to you. But need something like this because game freezes if divide by 0
		//We dont need to change (just) d, we should change tx/ty to actually get a high enough number to move him

		// After a hit, the object experiences knock back, where it is temporarily
		// under system control. This delivers two functions, the first being
		// a visual indicator to the player that something has happened, and the second
		// it stops the ability to spam attacks on a single creature
		victim->KnockBack(tx / d, ty / d, projectile);

		if (victim != m_pPlayer) 
		{
			// Quest reasons - i.e. enemy NPC - Could be to start battle
			// Other OnInteraction code above assumed object was friendly - This will allow interaction
			// with enemy objects, if need be
			// Most enemies will ignore this

			//Not using this at the moment because we're using OnInteract to register when an enemy has been touched, and that it will trigger for player to be hurt
			//victim->OnInteract(m_pPlayer);
		}
		else
		{
			// We must ensure the player is never pushed out of bounds by the physics engine. This
			// is a bit of a hack, but it allows knockbacks to occur providing there is an exit
			// point for the player to be knocked back into. If the player is "mobbed" then they
			// become trapped, and must fight their way out
			victim->bSolidVsDynamic = true;
		}

		if (projectile->bOneHit)
			projectile->bRedundant = true;
	}
}

bool Platformer::UpdateGameOver(float fElapsedTime)
{

		return true;
}