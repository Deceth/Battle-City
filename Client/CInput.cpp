#include "CInput.h"

#define KEYDOWN(Name,keY) (Name[keY] & 0X80)

/***************************************************************
 * Constructor
 *
 * @param Game
 **************************************************************/
CInput::CInput(CGame *Game) {
	p = Game;
	LastMouseX = 0;
	LastMouseY = 0;
	LastShot = 0;
	LastTankChange = 0;
	NeedRelease = 0;
	Tab = 0;
	MouseOverChat = 0;
	DemolishTimer = 0;
	lastRefresh = 0;
}

/***************************************************************
 * Destructor
 *
 **************************************************************/
CInput::~CInput() {
	// Tell DirectInput to release input devices 
	if(lpDI != NULL) {
		if(m_Keyboard != NULL) {
			m_Keyboard->Unacquire();
			m_Keyboard->Release();
		}
		if(m_Mouse != NULL) {
			m_Mouse->Unacquire();
			m_Mouse->Release();
		}
		lpDI->Release();
	}
}

/***************************************************************
 * ProcessKeys
 *
 * @param buffer
 **************************************************************/
void CInput::ProcessKeys(char buffer[256]) {

	// Get the user's index in the player list
	int me = p->Winsock->MyIndex;

	// If the player is dead, return
	if (p->Player[me]->isDead == true) {
		p->Player[me]->isMoving = false;
		p->Player[me]->isShooting = false;
		p->Player[me]->isTurning = false;
		return;
	}

	/************************************************
	 * Tab
	 ************************************************/
//	// Only allow Tab if not in 1024 mode
//	if( ! p->Options->resolution1024 ) {

		// If the user was previously holding TAB, see if TAB is still being held
		if (Tab == 1) {
			if (KEYDOWN(buffer, DIK_TAB)) {
				Tab = 1;
			}
			else {
				Tab = 0;
			}
		}
		// Else, if user is now holding TAB, center the screen
		else {
			if (KEYDOWN(buffer, DIK_TAB)) {
				Tab = 1;
			}
			else {
				Tab = 0;
			}
			if (Tab == 1) {
				p->Draw->resetPlayerOffset();
			}
		}
	
		// If showing the MiniMap, center the screen
		if (p->InGame->ShowMap) {
			Tab = 0;
		}
//	}

	int didTurn = 0;

	/************************************************
	 * Turning
	 ************************************************/
	// Key: RIGHT,
	if (KEYDOWN(buffer, DIK_RIGHT)) {
		// If holding TAB, move the screen
		if (Tab) {
			p->Draw->PlayerOffsetX -= (int)(10 * (p->TimePassed * 0.08));
			if (p->Draw->PlayerOffsetX < 0) p->Draw->PlayerOffsetX = 0;
			didTurn = 0;
		}
		// Else, set isTurning = 1 (RIGHT)
		else {
			p->Player[me]->isTurning = 1;
			didTurn = 1;
		}
	}

	// Key: LEFT
	if (KEYDOWN(buffer, DIK_LEFT)) {
		// If holding TAB, move the screen
		if (Tab) {
			p->Draw->PlayerOffsetX += (int)(10 * (p->TimePassed * 0.08));
			if (p->Draw->PlayerOffsetX > (p->Draw->MaxMapX-48)) p->Draw->PlayerOffsetX = (p->Draw->MaxMapX-48);
			didTurn = 0;
		}
		// Else, set isTurning = -1 (LEFT)
		else {
			p->Player[me]->isTurning = -1;
			didTurn = 1;
		}
	}
	
	// If the user turned, reset isTurning to 0
	if (didTurn == 0) {
		p->Player[me]->isTurning = 0;
	}

	/************************************************
	 * Movement
	 ************************************************/
	int didMove = 0;
	int WasMoving = p->Player[me]->isMoving;

	// Key: DOWN
	if (KEYDOWN(buffer, DIK_DOWN)) {
		// If holding TAB, move the screen
		if (Tab) {
			p->Draw->PlayerOffsetY -= (int)(10 * (p->TimePassed * 0.08));
			if (p->Draw->PlayerOffsetY < 0) {
				p->Draw->PlayerOffsetY = 0;
			}
			didMove = 0;
		}
		// Else,
		else {
			// If isFrozen, don't move
			if (p->Player[me]->isFrozen) {
				didMove = 0;
			}
			// Else, not isFrozen, set isMoving = -1 (BACKWARD)
			else {
				didMove = 1; 
				p->Player[me]->isMoving = -1;
			}
		}
	}

	// Key: UP
	if (KEYDOWN(buffer, DIK_UP)) {
		// If holding TAB, move the screen
		if (Tab) {
			p->Draw->PlayerOffsetY += (int)(10 * (p->TimePassed * 0.08));
			if (p->Draw->PlayerOffsetY > (p->Draw->MaxMapY-48)) {
				p->Draw->PlayerOffsetY = (p->Draw->MaxMapY-48);
			}
			didMove = 0;
		}
		// Else,
		else {
			// If isFrozen, don't move
			if (p->Player[me]->isFrozen) {
				didMove = 0;
			}
			// Else, not isFrozen, set isMoving = 1 (FORWARD)
			else {
				didMove = 1; 
				p->Player[me]->isMoving = 1;
			}
		}
	}

	// If the user did not move,
	if (didMove == 0)  {
		p->Player[me]->isMoving = 0;
		
		// If the user was moving before, stop tank sound and stop the player's movement
		if (WasMoving) {
			p->Sound->StopWav(12);
			sCMUpdate packet;
			packet.x = (int)p->Player[me]->X;
			packet.y = (int)p->Player[me]->Y;
			packet.dir = (unsigned char)p->Player[me]->Direction;
			packet.move = (unsigned char)(p->Player[me]->isMoving + 1);
			packet.turn = (unsigned char)(p->Player[me]->isTurning + 1);
			p->Winsock->SendData(cmUpdate, (char *)&packet, sizeof(packet));
		}
	}
	// Else, user moved,
	else {
		// If the user wasn't moving before (and has tankSound on), play tank sound
		if (WasMoving == 0 && p->Options->tanksound == 1) {
			p->Sound->PlayWav(sEngine, 12);
		}
	}

	/************************************************
	 * If Player Is Not Chatting
	 ************************************************/
	if (p->InGame->IsChatting == 0) {

		/************************************************
		 * Switching Tanks
		 ************************************************/
		// Keys: 0-9
		if (KEYDOWN(buffer, DIK_0)) {
			this->ChangeTank(0);
		}
		else if (KEYDOWN(buffer, DIK_1)) {
			this->ChangeTank(1);
		}
		else if (KEYDOWN(buffer, DIK_2)) {
			this->ChangeTank(2);
		}
		else if (KEYDOWN(buffer, DIK_3)) {
			this->ChangeTank(3);
		}
		else if (KEYDOWN(buffer, DIK_4)) {
			this->ChangeTank(4);
		}
		else if (KEYDOWN(buffer, DIK_5)) {
			this->ChangeTank(5);
		}
		else if (KEYDOWN(buffer, DIK_6)) {
			this->ChangeTank(6);
		}
		else if (KEYDOWN(buffer, DIK_7)) {
			this->ChangeTank(7);
		}
		else if (KEYDOWN(buffer, DIK_8)) {
			this->ChangeTank(8);
		}
		else if (KEYDOWN(buffer, DIK_9)) {
			this->ChangeTank(9);
		}

		/************************************************
		 * Firing
		 ************************************************/
		// Key: SHIFT (and user isn't chatting)
		if ((KEYDOWN(buffer, DIK_LSHIFT) || KEYDOWN(buffer, DIK_RSHIFT))) {

			// If the firing timer allows another shot (and user isn't frozen)
			if ((p->Tick > this->LastShot) && (p->Player[me]->isFrozen == 0)) {

				// Weapon: ADMIN
				if (p->Player[me]->isAdmin()) {
					this->LastShot = p->Tick + TIMER_SHOOT_ADMIN;

					float fDir = (float)-p->Player[me]->Direction+32;
					int FlashY = (int)p->Player[me]->Y-24+10 + (int)(cos((float)(fDir)/16*3.14)*20);
					int FlashX = (int)p->Player[me]->X-24+6 + (int)(sin((float)(fDir)/16*3.14)*20);
					p->Explode->newExplosion(FlashX, FlashY, 3);
					int adminShot = 2;
					p->Bullet->newBullet(FlashX, FlashY, adminShot, p->Player[me]->Direction, me);
					p->Sound->PlayWav(sTurretfire,2);
					sCMShot shot;
					shot.dir = p->Player[me]->Direction;
					shot.type = adminShot;
					shot.x = FlashX;
					shot.y = FlashY;
					p->Winsock->SendData(cmShoot, (char *)&shot, sizeof(shot));
				}

				// Weapon: ROCKET (and user isn't moving)
				else if (p->InGame->HasRocket && (p->Player[me]->isMoving == 0)) {
					this->LastShot = p->Tick + TIMER_SHOOT_ROCKET;

					float fDir = (float)-p->Player[me]->Direction+32;
					int FlashY = (int)p->Player[me]->Y-24+10 + (int)(cos((float)(fDir)/16*3.14)*20);
					int FlashX = (int)p->Player[me]->X-24+6 + (int)(sin((float)(fDir)/16*3.14)*20);
					p->Explode->newExplosion(FlashX, FlashY, 3);
					p->Bullet->newBullet(FlashX, FlashY, 1, p->Player[p->Winsock->MyIndex]->Direction, me);
					p->Sound->PlayWav(sBigturret,2);
					sCMShot shot;
					shot.dir = p->Player[me]->Direction;
					shot.type = 1;
					shot.x = FlashX;
					shot.y = FlashY;
					p->Winsock->SendData(cmShoot, (char *)&shot, sizeof(shot));
				}

				// Weapon: LASER
				else if (p->InGame->HasLaser) {
					this->LastShot = p->Tick + TIMER_SHOOT_LASER;

					float fDir = (float)-p->Player[me]->Direction+32;
					int FlashY = (int)p->Player[me]->Y-24+10 + (int)(cos((float)(fDir)/16*3.14)*20);
					int FlashX = (int)p->Player[me]->X-24+6 + (int)(sin((float)(fDir)/16*3.14)*20);
					p->Explode->newExplosion(FlashX, FlashY, 3);
					p->Bullet->newBullet(FlashX, FlashY, 0, p->Player[me]->Direction, me);
					p->Sound->PlayWav(sLaser,1);
					sCMShot shot;
					shot.dir = p->Player[me]->Direction;
					shot.type = 0;
					shot.x = FlashX;
					shot.y = FlashY;
					p->Winsock->SendData(cmShoot, (char *)&shot, sizeof(shot));
				}
				
				// Ability to shoot laser even without having the item.
				else {
					this->LastShot = p->Tick + TIMER_SHOOT_LASER;

					float fDir = (float)-p->Player[me]->Direction+32;
					int FlashY = (int)p->Player[me]->Y-24+10 + (int)(cos((float)(fDir)/16*3.14)*20);
					int FlashX = (int)p->Player[me]->X-24+6 + (int)(sin((float)(fDir)/16*3.14)*20);
					p->Explode->newExplosion(FlashX, FlashY, 3);
					p->Bullet->newBullet(FlashX, FlashY, 0, p->Player[me]->Direction, me);
					p->Sound->PlayWav(sLaser,1);
					sCMShot shot;
					shot.dir = p->Player[me]->Direction;
					shot.type = 0;
					shot.x = FlashX;
					shot.y = FlashY;
					p->Winsock->SendData(cmShoot, (char *)&shot, sizeof(shot));
				}

				/* Commented out while testing ability to always have laser
				// Weapon: NONE (and Newbie Tips are on)
				else if (p->Options->newbietips == 1) {
					p->InGame->NewbieTip = "You cannot fire until you pick up a Laser or Cougar Missle.";
				}*/
			}
		}
	}
}

/***************************************************************
 * MouseMove
 *
 * @param mouse_state
 * @param X
 * @param Y
 * @param buffer
 **************************************************************/
void CInput::MouseMove(DIMOUSESTATE mouse_state, int X, int Y, char buffer[256]) {
	X -= p->DDraw->XOff;
	Y -= p->DDraw->YOff;
	this->LastMouseX = X;
	this->LastMouseY = Y;
	if (this->LastMouseX < 0) this->LastMouseX = 0;
	if (this->LastMouseY < 0) this->LastMouseY = 0;
	if (X < p->Draw->MaxMapX && Y > (p->Draw->MaxMapY - 84)) MouseOverChat = 1; else MouseOverChat = 0;
}

/***************************************************************
 * MouseUp
 *
 * @param mouse_state
 * @param X
 * @param Y
 * @param buffer
 **************************************************************/
void CInput::MouseUp(DIMOUSESTATE mouse_state, int X, int Y, char buffer[256]) {
	// Set class variable NeedRelease to 0 to show that mouse button was released
	NeedRelease = 0;
}

/***************************************************************
 * MouseDown
 *
 * @param mouse_state
 * @param X
 * @param Y
 * @param buffer
 **************************************************************/
void CInput::MouseDown(DIMOUSESTATE mouse_state, int X, int Y, char buffer[256]) {
	X -= p->DDraw->XOff;
	Y -= p->DDraw->YOff;
	
	// If we're ignoring all actions until the player lets go of the button, return
	if (NeedRelease) {
		return;
	}

	int me = p->Winsock->MyIndex;
	int myX = (int)p->Player[me]->X / 48;
	int myY = (int)p->Player[me]->Y / 48;
	int tX = p->Input->LastMouseX / 48;
	int tY = p->Input->LastMouseY / 48;
	int Xloc = myX - tX + p->Draw->clickOffsetX;
	int Yloc = myY - tY + p->Draw->clickOffsetY;

	/************************************************
	 * Button: LEFT
	 ************************************************/
	if (mouse_state.rgbButtons[0] & 0X80) {

		// If the player is building/demolishing,
		if (p->InGame->IsBuilding != 0) {

			/************************************************
			* Demolishing
			************************************************/
			if (p->InGame->IsBuilding == -1) {

				// If the demolish timer is up,
// COMMENTED OUT: Demo Timer
/*
				if (p->Tick > DemolishTimer) {
*/
					CBuilding *TestBuilding;

					// Check whether there is a building under the cursor
					for (int j = 0; j < 3; j++) {
						for (int k = 0; k < 3; k++) {
							TestBuilding = p->Build->findBuildingbyLocation((Xloc-1) + j, (Yloc-1) + k);

							// If there is a building under the cursor,
							if (TestBuilding) {
								
								// Tell the server to demolish the building
								sCMDemolish d_packet;
								d_packet.id = TestBuilding->id;
								p->Winsock->SendData(cmDemolish,(char *)&d_packet,sizeof(d_packet));
								NeedRelease = 1;

								// Reset the demolish timer
// COMMENTED OUT: Demo Timer
/*
								if (p->Tick - DemolishTimer < 1000) {
									DemolishTimer = p->Tick + TIMER_DEMOLISH;
								}
								else {
									DemolishTimer = p->Tick;
								}
*/

								// Save the mouse state and return
								this->endMouseDown(mouse_state);
								return;
							}
						}
					}
// COMMENTED OUT: Demo Timer
/*
				}
*/
			}

			/************************************************
			* Building
			************************************************/
			else {

				// If the player doesn't have enough money to build (and isn't an admin), return
				if (p->InGame->Cash < COST_BUILDING && (p->Player[p->Winsock->MyIndex]->isAdmin() == false)) {
					return;
				}

				// ???
				if (p->InGame->IsBuilding == 0 && p->InGame->Cash < 2000000) {
					return;
				}
				
				// If there is nothing under the building (or the user is an admin),
				if (p->Collision->CheckBuildingCollision(Xloc, Yloc) == 0 || p->Player[p->Winsock->MyIndex]->isAdmin()) {
					
					// If the building is in the city's build range,
					if (p->Build->inRange() == 1) {

						// ???
						// If the building is on the map
						if (Xloc > 0 && Xloc < 510 && Yloc > 0 && Yloc < 510) {

							// Tell the server to build the building
							sCMBuild b_packet;
							b_packet.x = Xloc;
							b_packet.y = Yloc;
							b_packet.type = (unsigned char)p->InGame->IsBuilding;
							p->Winsock->SendData(cmBuild,(char *)&b_packet,sizeof(b_packet));

							// Clear the building off the cursor
							p->InGame->IsBuilding = 0;

							// Save the mouse state and return
							this->endMouseDown(mouse_state);
							return;
						}
					}
				}
			}
		}

		/************************************************
		* Build Menu
		************************************************/
		if (p->InGame->ShowBuildMenu) {

			// Clear the build menu
			p->InGame->ShowBuildMenu = 0;

			// If the player clicked inside the build menu,
			if (X > p->Draw->BuildMenuX-26 && X < p->Draw->BuildMenuX+180 && Y < p->Draw->BuildMenuY+16) {
				p->Draw->resetPlayerOffset();

				int Ym = p->Draw->BuildMenuY+16;

				// If the player clicked DEMOLISH,
				if (Y > Ym -16 && Y < Ym) {

					// Set the player in demolish mode,
					p->InGame->IsBuilding = -1;

					// Save the mouse state and return
					this->endMouseDown(mouse_state);
					return;
				}

				// Else (player clicked a building), for each type of building,
				for (int j = 0; j < 26; j++) {

					// If the player can build that building (or the player is an admin),
					if (p->InGame->CanBuild[j] || p->Player[p->Winsock->MyIndex]->isAdmin()) {
						Ym -= 16;

						// If the building is the one the player just clicked in the build menu,
						if (Y > Ym -16 && Y < Ym) {

							// Set IsBuilding to the type of building the player clicked
							p->InGame->IsBuilding = j+1;

							// If newbie tips are on,
							if (p->Options->newbietips == 1) {
								// Building: HOSPITAL
								if (p->InGame->IsBuilding == 1) {
									p->InGame->NewbieTip = "Stand on a hospital to restore your health! Hospitals are expensive to maintain so if you are running low on money, demolish it.";
								}
								// Building: HOUSE
								else if (p->InGame->IsBuilding == 2) {
									p->InGame->NewbieTip = "Houses are required for your city population. For every 2 non-housing buildings you construct, you will need 1 house.";
								}
								// Building: RESEARCH
								else if (p->InGame->IsBuilding % 2) {
									 p->InGame->NewbieTip = "After research is complete, you will be able to build a factory which will produce that item.";
								}
								// Building: FACTORY
								else {
									 p->InGame->NewbieTip = "Factories produce items. Press 'u' while on top of the item to pick it up. Press 'd' to drop the item.";
								}
							}

							// Save the mouse state and return
							this->endMouseDown(mouse_state);
							return;
						}	
					}
				}
			}
		}


		/************************************************
		* Admin Button
		************************************************/
		// If player is an admin, and player clicked admin button, show admin dialog
		if (p->Player[p->Winsock->MyIndex]->isAdmin()) {
			if (X > (p->Draw->MaxMapX + 110) && X < (p->Draw->MaxMapX + 110 + 42) && Y > 460 && Y < 479) {
				p->Admin->ShowAdminDlg();

				// Save the mouse state and return
				this->endMouseDown(mouse_state);
				return;
			}
		}

		/************************************************
		* Inventory
		************************************************/
		// If user clicked an inventory box, set that item to Selected
		if ((X > (p->Draw->MaxMapX + 7)) && (X < (p->Draw->MaxMapX + 109)) && (Y > 267) && (Y < 404)) {
			p->Inventory->SelectedItemType = ((X - p->Draw->MaxMapX - 7) / 35);
			p->Inventory->SelectedItemType = (p->Inventory->SelectedItemType + (((Y - 267) / 35) * 3));

			// Save the mouse state and return
			this->endMouseDown(mouse_state);
			return;
		}

		/************************************************
		* Panel
		************************************************/
		if (X > p->Draw->MaxMapX) {

			// Main Buttons
			if ((X > (p->Draw->MaxMapX+145)) && (X < (p->Draw->MaxMapX+190)) && (Y < 420)) {
				
				// Staff button
				if (Y > 268 && Y < 288) {

					// If player is Mayor, open PersonnelDialog
					if (p->Player[p->Winsock->MyIndex]->isMayor) {
						p->Winsock->SendData(cmComms, "|");
						p->Personnel->ShowPersonnelDialog();

						// Save the mouse state and return
						this->endMouseDown(mouse_state);
						return;
					}
				}

				// Map button
				if (Y > 290 && Y < 310) {

					// Toggle whether the map is open
					p->InGame->ShowMap = 1 - p->InGame->ShowMap;

					// Force the player to stop
					p->Player[p->Winsock->MyIndex]->isMoving = 0;

					// If the map is now open, request the minimap from the server
					if (p->InGame->ShowMap == 1){
						p->Winsock->SendData(cmMiniMap, " ");
					}

					// Save the mouse state and return
					this->endMouseDown(mouse_state);
					return;
				}

				// Info button
				if (Y > 312 && Y < 332) {
					this->InfoButton();

					// Save the mouse state and return
					this->endMouseDown(mouse_state);
					return;
				}

				// Points button
				if (Y > 334 && Y < 354) {
					PointsButton();

					// Save the mouse state and return
					this->endMouseDown(mouse_state);
					return;
				}

				// Options button
				if (Y > 356 && Y < 376) {
					p->Options->ShowOptionsDialog();

					// Save the mouse state and return
					this->endMouseDown(mouse_state);
					return;
				}

				// Help button
				if (Y > 378 && Y < 398) {
					p->Help->ShowHelpDialog();

					// Save the mouse state and return
					this->endMouseDown(mouse_state);
					return;
				}
			}

			// Build button
			if ((Y > 400) && (Y < 422) && (X > (p->Draw->MaxMapX + 126)) && (X < (p->Draw->MaxMapX + 190))) {
				p->Draw->BuildMenuX = p->Draw->MaxMapX - 180;
				p->Draw->BuildMenuY = 344;
				p->InGame->ShowBuildMenu = 1;

				// Save the mouse state and return
				this->endMouseDown(mouse_state);
				return;
			}

			// Quit button
			if ((Y > 576) && (Y < 594) && (X > (p->Draw->MaxMapX + 150)) && (X < (p->Draw->MaxMapX + 192))) {
				p->Engine->ThreadMessageQuit();

				// Save the mouse state and return
				this->endMouseDown(mouse_state);
				return;
			}
	
			// Money box
			if ((X > p->Draw->MaxMapX + 2) && (X < p->Draw->MaxMapX + 2 + 120) && (Y > 224) && (Y < 243)) {
				
				// If player is mayor, show finance report
				if (p->Player[p->Winsock->MyIndex]->isMayor) {
					p->InGame->PrintFinanceReport();

					// Save the mouse state and return
					this->endMouseDown(mouse_state);
					return;
				}
			}
		}
	}

	/************************************************
	 * Button: RIGHT
	 ************************************************/
	if (mouse_state.rgbButtons[1] & 0X80) {
		
		/************************************************
		* Inventory
		************************************************/
		if ((X > (p->Draw->MaxMapX + 7)) && (X < (p->Draw->MaxMapX + 109)) && (Y > 267) && (Y < 404)) {
			p->Inventory->SelectedItemType = ((X - p->Draw->MaxMapX - 7) / 35);
			p->Inventory->SelectedItemType = (p->Inventory->SelectedItemType + (((Y - 267) / 35) * 3));

			// Item: MEDKIT
			if (p->Inventory->SelectedItemType == 2)  {

				// Find the medkit and tell the server to use it
				CItem *itm = p->Inventory->findItembyType(2);
				if (itm) {
					p->Winsock->SendData(cmMedKit, (char *)&itm->id, sizeof(itm->id));

					// Save the mouse state and return
					this->endMouseDown(mouse_state);
					return;
				}
			}

			// Item: BOMB
			if (p->Inventory->SelectedItemType == 3) {

				// Activate the bomb stack
				p->InGame->BombsAreActivated = 1 - p->InGame->BombsAreActivated;

				// Save the mouse state and return
				this->endMouseDown(mouse_state);
				return;
			}
		}

		/************************************************
		* Money Box
		************************************************/
		if ((X > p->Draw->MaxMapX + 2) && (X < p->Draw->MaxMapX + 2 + 120) && (Y > 224) && (Y < 243)) {

			// If player is mayor, show finance report
			if (p->Player[p->Winsock->MyIndex]->isMayor) {
				p->InGame->PrintFinanceReport();

				// Save the mouse state and return
				this->endMouseDown(mouse_state);
				return;
			}
		}

		/************************************************
		* Map
		************************************************/
		if (X >= 0 && X <= p->Draw->MaxMapX && Y >= 0 && Y <= p->Draw->MaxMapY) {
			p->Draw->resetPlayerOffset();
			Rect rct1, rct2;

			/************************************************
			* Click Player
			************************************************/
			// Check whether there is a player under the cursor
			for (int i = 0; i < MAX_PLAYERS; i++) {
				rct1.X = (int)(p->Player[me]->X - p->Player[i]->X + p->Draw->PlayerOffsetX);
				rct1.Y = (int)(p->Player[me]->Y - p->Player[i]->Y + p->Draw->PlayerOffsetY);
				rct1.w = 48;
				rct1.h = 48;

				rct2.X = p->Input->LastMouseX;
				rct2.Y = p->Input->LastMouseY;
				rct2.w = 1;
				rct2.h = 1;
				
				// If there is a player under the cursor,
				if (p->Collision->RectCollision(rct1, rct2)) {
					if (p->Engine->MsgQueue == 0) {
						p->Engine->MsgQueue = 1;

						// Ask the server for the player's data
						char packet[2];
						packet[0] = i;
						packet[1] = 0;
						p->Winsock->SendData(cmClickPlayer, packet, 1);

						// Save the mouse state and return
						this->endMouseDown(mouse_state);
						return;
					}
				}
			}

			/************************************************
			* Click Building
			************************************************/
			// Check whether there is a building under the cursor,
			CBuilding *TestBuilding;
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					TestBuilding = p->Build->findBuildingbyLocation((Xloc-1) + j, (Yloc-1) + k);

					// If there is a building under the cursor,
					if (TestBuilding) {
						
						// Ask the server for the building's city's data
						char packet[2];
						packet[0] = TestBuilding->City;
						packet[1] = 0;
						p->Winsock->SendData(cmRightClickCity, packet, sizeof(packet));

						// Save the mouse state and return
						this->endMouseDown(mouse_state);
						return;
					}
				}
			}

			/************************************************
			* Build Menu
			************************************************/
			// If player is mayor,
			if (p->Player[p->Winsock->MyIndex]->isMayor) {

				// Show build menu
				p->InGame->IsBuilding = 0;
				p->InGame->ShowBuildMenu = 1;
				p->Draw->BuildMenuX = X;
				p->Draw->BuildMenuY = Y;

				// Force build menu to be in inside screen
				if (X < 16) {
					p->Draw->BuildMenuX = 16;
				}
				if (Y > (p->ResolutionY - 16)) {
					p->Draw->BuildMenuY = p->ResolutionY - 16;
				}

				// Save the mouse state and return
				this->endMouseDown(mouse_state, false);
				return;
			}
		}
	}
	
	// Save the mouse state and return
	this->endMouseDown(mouse_state);
	return;
}

/***************************************************************
 * endMouseDown
 *
 * Helper function for MouseDown:
 *  - Saves the mouse state
 *  - Sets NeedRelease = 1
 *
 * @param mouse_state
 **************************************************************/
void CInput::endMouseDown(DIMOUSESTATE mouse_state) {
	this->endMouseDown(mouse_state, true);
}

/***************************************************************
 * endMouseDown
 *
 * Helper function for MouseDown:
 *  - Saves the mouse state
 *  - If setNeedRelease is true, sets NeedRelease = 1
 *
 * @param mouse_state
 * @param setNeedRelease
 **************************************************************/
void CInput::endMouseDown(DIMOUSESTATE mouse_state, bool setNeedRelease) {
    oMouseState.rgbButtons[0] = mouse_state.rgbButtons[0];
    oMouseState.rgbButtons[1] = mouse_state.rgbButtons[1];
	if (setNeedRelease) {
		this->NeedRelease = 1;
	}
}

/***************************************************************
 * StartDInput
 *
 * Uses DirectInput to acquire input devices
 **************************************************************/
void CInput::StartDInput() {
	DirectInput8Create(p->hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&lpDI, NULL);

	lpDI->CreateDevice(GUID_SysKeyboard, &m_Keyboard, NULL);
	m_Keyboard->SetDataFormat(&c_dfDIKeyboard);
	m_Keyboard->SetCooperativeLevel(p->hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_Keyboard->Acquire();

	lpDI->CreateDevice(GUID_SysMouse, &m_Mouse, NULL);
	m_Mouse->SetDataFormat(&c_dfDIMouse);
	m_Mouse->SetCooperativeLevel(p->hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_Mouse->Acquire();
}

/***************************************************************
 * Cycle
 *
 * Uses DirectInput to acquire input devices
 **************************************************************/
void CInput::Cycle() {

	// If we're missing any input devices, or the window doesn't have focus, return
	if ((!m_Keyboard) || (!m_Mouse) || (!HasFocus)) {
		return;
	}

	char buffer[256];
	DIMOUSESTATE mouse_state;
	
	m_Keyboard->GetDeviceState(sizeof(buffer),&buffer);
	m_Mouse->GetDeviceState(sizeof(DIMOUSESTATE),&mouse_state);

    RECT R1;
    GetWindowRect(p->hWnd, &R1);
    POINT tMouse;
    GetCursorPos(&tMouse);
    Mouse.x = tMouse.x - R1.left;
    Mouse.y = tMouse.y - R1.top;

	// If the mouse has moved, call MouseMove and store the new position
	if ((Mouse.x != oMouse.x) || (Mouse.y != oMouse.y)) {
		this->MouseMove(mouse_state, Mouse.x, Mouse.y, buffer);
		oMouse.x = Mouse.x;
		oMouse.y = Mouse.y;
	}

	// The bit-wise "& 128" operations below look intentional-- not an error

	// If either mouse button is pressed, call MouseDown
	if ((mouse_state.rgbButtons[0] & 128) || (mouse_state.rgbButtons[1] & 128)) {
		this->MouseDown(mouse_state, Mouse.x, Mouse.y, buffer);
	}

	// If either mouse button was pressed before, but is now released, call MouseUp
	if (
			( (oMouseState.rgbButtons[0] & 128) && (!(mouse_state.rgbButtons[0] & 128)) )
			||
			( (oMouseState.rgbButtons[1] & 128) && (!(mouse_state.rgbButtons[1] & 128)) )
		) {
		this->MouseUp(mouse_state, Mouse.x, Mouse.y, buffer);
	}

	// Save the mouse_state as oMouseState
	oMouseState = mouse_state;

	// Call ProcessKeys
	this->ProcessKeys(buffer);
}

/***************************************************************
 * InfoButton
 *
 **************************************************************/
void CInput::InfoButton() {
	p->InGame->PrintWhoData();

	p->Winsock->SendData(cmRequestInfo, " ");
}

/***************************************************************
 * PointsButton
 *
 **************************************************************/
void CInput::PointsButton() {
	std::string PointString;
	std::ostringstream thing;
	PointString = "Points: ";
	thing << p->Player[p->Winsock->MyIndex]->Points;
	PointString += thing.str();
	thing.str("");
	PointString += "   Orbs: ";
	thing << p->Player[p->Winsock->MyIndex]->Orbs;
	PointString += thing.str();
	thing.str("");
	PointString += "   Assists: ";
	thing << p->Player[p->Winsock->MyIndex]->Assists;
	PointString += thing.str();
	thing.str("");
	PointString += "   Deaths: ";
	thing << p->Player[p->Winsock->MyIndex]->Deaths;
	PointString += thing.str();
	thing.str("");
	PointString += "   Points this month: ";
	thing << p->Player[p->Winsock->MyIndex]->MonthlyPoints;
	PointString += thing.str();
	p->InGame->AppendChat(PointString.c_str(), RGB(255, 215, 0));
}

/***************************************************************
 * ChangeTank
 * 
 * Helper method to handle tank changing
 **************************************************************/
void CInput::ChangeTank(int tankNumber) {

	// If the tank switch timer allows another switch,
	if (p->Tick > this->LastTankChange) {
		this->LastTankChange = p->Tick + TIMER_CHANGE_TANK;
		
		// Tell the server to change your tank (has to be server-side so everyone sees the change)
		char packet[2];
		packet[0] = tankNumber;
		packet[1] = 0;
		p->Winsock->SendData(cmChangeTank, packet, 1);
	}
}