#include "CInput.h"

#define KEYDOWN(Name,keY) (Name[keY] & 0X80)

CInput::CInput(CGame *Game)
{
	p = Game;
	LastMouseX = 0;
	LastMouseY = 0;
	LastShot = 0;
	NeedRelease = 0;
	Tab = 0;
	MouseOverChat = 0;

	DemolishTimer = 0;

	lastRefresh = 0;
}

CInput::~CInput()
{
	if(lpDI != NULL)
	{
		if(m_Keyboard != NULL)
		{
			m_Keyboard->Unacquire();
			m_Keyboard->Release();
		}
		if(m_Mouse != NULL)
		{
			m_Mouse->Unacquire();
			m_Mouse->Release();
		}
		lpDI->Release();
	}
}

void CInput::ProcessKeys(char buffer[256])
{
	if (p->Player[p->Winsock->MyIndex]->isDead == true) 
	{
		p->Player[p->Winsock->MyIndex]->isMoving = false;
		p->Player[p->Winsock->MyIndex]->isShooting = false;
		p->Player[p->Winsock->MyIndex]->isTurning = false;
		return;
	}

	int me = p->Winsock->MyIndex;

	if (Tab == 1)
		if(KEYDOWN(buffer, DIK_TAB)) Tab = 1; else Tab = 0;
	else
	{
		if(KEYDOWN(buffer, DIK_TAB)) Tab = 1; else Tab = 0;
		if (Tab == 1)
		{
			p->Draw->PlayerOffsetX = (p->Draw->MaxMapX / 2) - 24;
			p->Draw->PlayerOffsetY = (p->Draw->MaxMapY / 2) - 24;
		}
	}

	if (p->InGame->ShowMap) Tab = 0;

	int didTurn = 0;
	if(KEYDOWN(buffer, DIK_RIGHT))
	{
		if (Tab)
		{
			p->Draw->PlayerOffsetX -= (int)(10 * (p->TimePassed * 0.08));
			if (p->Draw->PlayerOffsetX < 0) p->Draw->PlayerOffsetX = 0;
			didTurn = 0;
		}
		else
		{
			p->Player[me]->isTurning = 1;
			didTurn = 1;
		}
	}
	if(KEYDOWN(buffer, DIK_LEFT))
	{
		if (Tab)
		{
			p->Draw->PlayerOffsetX += (int)(10 * (p->TimePassed * 0.08));
			if (p->Draw->PlayerOffsetX > (p->Draw->MaxMapX-48)) p->Draw->PlayerOffsetX = (p->Draw->MaxMapX-48);
			didTurn = 0;
		}
		else
		{
			p->Player[me]->isTurning = -1;
			didTurn = 1;
		}
	}
	if (didTurn == 0) p->Player[me]->isTurning = 0;

	int didMove = 0;
	int WasMoving = p->Player[me]->isMoving;
	if(KEYDOWN(buffer, DIK_DOWN)) 
	{
		if (Tab)
		{
			p->Draw->PlayerOffsetY -= (int)(10 * (p->TimePassed * 0.08));
			if (p->Draw->PlayerOffsetY < 0) p->Draw->PlayerOffsetY = 0;
			didMove = 0;
		}
		else
		{
			if (p->Player[me]->isFrozen)
			{
				didMove = 0;
			}
			else
			{
				didMove = 1; 
				p->Player[me]->isMoving = -1;
			}
		}
	}
	if(KEYDOWN(buffer, DIK_UP)) 
	{
		if (Tab)
		{
			p->Draw->PlayerOffsetY += (int)(10 * (p->TimePassed * 0.08));
			if (p->Draw->PlayerOffsetY > (p->Draw->MaxMapY-48)) p->Draw->PlayerOffsetY = (p->Draw->MaxMapY-48);
			didMove = 0;
		}
		else
		{
			if (p->Player[me]->isFrozen)
			{
				didMove = 0;
			}
			else
			{
				didMove = 1; 
				p->Player[me]->isMoving = 1;
			}
		}
	}
	if (didMove == 0) 
	{
		p->Player[me]->isMoving = 0;
		if (WasMoving) 
		{
			p->Sound->StopWav(12);
			sCMUpdate packet;
			int me = p->Winsock->MyIndex;
			packet.x = (int)p->Player[me]->X;
			packet.y = (int)p->Player[me]->Y;
			packet.dir = (unsigned char)p->Player[me]->Direction;
			packet.move = (unsigned char)(p->Player[me]->isMoving + 1);
			packet.turn = (unsigned char)(p->Player[me]->isTurning + 1);
			p->Winsock->SendData(cmUpdate, (char *)&packet, sizeof(packet));
		}
	}
	else
	{
		if (WasMoving == 0 && p->Options->tanksound == 1) p->Sound->PlayWav(sEngine, 12);
	}

	if((KEYDOWN(buffer, DIK_LSHIFT) || KEYDOWN(buffer, DIK_RSHIFT)) && p->InGame->IsChatting == 0) {
		if (p->Tick > this->LastShot && p->Player[p->Winsock->MyIndex]->isFrozen == 0)
		{
			this->LastShot = p->Tick + 650;
			if (p->Player[p->Winsock->MyIndex]->isAdmin == 2)
			{
				float fDir = (float)-p->Player[p->Winsock->MyIndex]->Direction+32;
				int FlashY = (int)p->Player[p->Winsock->MyIndex]->Y-24+10 + (int)(cos((float)(fDir)/16*3.14)*20);
				int FlashX = (int)p->Player[p->Winsock->MyIndex]->X-24+6 + (int)(sin((float)(fDir)/16*3.14)*20);
				p->Explode->newExplosion(FlashX, FlashY, 3);
				int adminShot = 2;
				p->Bullet->newBullet(FlashX, FlashY, adminShot, p->Player[p->Winsock->MyIndex]->Direction, p->Winsock->MyIndex);
				p->Sound->PlayWav(sTurretfire,2);
				sCMShot shot;
				shot.dir = p->Player[p->Winsock->MyIndex]->Direction;
				shot.type = adminShot;
				shot.x = FlashX;
				shot.y = FlashY;
				p->Winsock->SendData(cmShoot, (char *)&shot, sizeof(shot));
				this->LastShot -= 600;
			}
			if (p->InGame->HasRocket)
			{
				if (p->Player[p->Winsock->MyIndex]->isMoving == 0)
				{
					float fDir = (float)-p->Player[p->Winsock->MyIndex]->Direction+32;
					int FlashY = (int)p->Player[p->Winsock->MyIndex]->Y-24+10 + (int)(cos((float)(fDir)/16*3.14)*20);
					int FlashX = (int)p->Player[p->Winsock->MyIndex]->X-24+6 + (int)(sin((float)(fDir)/16*3.14)*20);
					p->Explode->newExplosion(FlashX, FlashY, 3);
					p->Bullet->newBullet(FlashX, FlashY, 1, p->Player[p->Winsock->MyIndex]->Direction, p->Winsock->MyIndex);
					p->Sound->PlayWav(sBigturret,2);
					sCMShot shot;
					shot.dir = p->Player[p->Winsock->MyIndex]->Direction;
					shot.type = 1;
					shot.x = FlashX;
					shot.y = FlashY;
					p->Winsock->SendData(cmShoot, (char *)&shot, sizeof(shot));
					return;
				}
			}
			if (p->InGame->HasLaser)
			{
				float fDir = (float)-p->Player[p->Winsock->MyIndex]->Direction+32;
				int FlashY = (int)p->Player[p->Winsock->MyIndex]->Y-24+10 + (int)(cos((float)(fDir)/16*3.14)*20);
				int FlashX = (int)p->Player[p->Winsock->MyIndex]->X-24+6 + (int)(sin((float)(fDir)/16*3.14)*20);
				p->Explode->newExplosion(FlashX, FlashY, 3);
				p->Bullet->newBullet(FlashX, FlashY, 0, p->Player[p->Winsock->MyIndex]->Direction, p->Winsock->MyIndex);
				p->Sound->PlayWav(sLaser,1);
				sCMShot shot;
				shot.dir = p->Player[p->Winsock->MyIndex]->Direction;
				shot.type = 0;
				shot.x = FlashX;
				shot.y = FlashY;
				p->Winsock->SendData(cmShoot, (char *)&shot, sizeof(shot));
				return;
			}
		}
	}
}

void CInput::MouseMove(DIMOUSESTATE mouse_state, int X, int Y, char buffer[256])
{
	X -= p->DDraw->XOff;
	Y -= p->DDraw->YOff;
	this->LastMouseX = X;
	this->LastMouseY = Y;
	if (this->LastMouseX < 0) this->LastMouseX = 0;
	if (this->LastMouseY < 0) this->LastMouseY = 0;
	if (X < p->Draw->MaxMapX && Y > (p->Draw->MaxMapY - 84)) MouseOverChat = 1; else MouseOverChat = 0;
}

void CInput::MouseUp(DIMOUSESTATE mouse_state, int X, int Y, char buffer[256])
{
	NeedRelease = 0;
}

void CInput::MouseDown(DIMOUSESTATE mouse_state, int X, int Y, char buffer[256])
{
	X -= p->DDraw->XOff;
	Y -= p->DDraw->YOff;
	if (NeedRelease) return;

	if (mouse_state.rgbButtons[0] & 0X80) //Left 
    {
		if (p->InGame->IsBuilding != 0)
		{
			int Xloc, Yloc;

			int me = p->Winsock->MyIndex;
			int mYX = (int)p->Player[me]->X / 48;
			int mYY = (int)p->Player[me]->Y / 48;
			int tX = p->Input->LastMouseX / 48;
			int tY = p->Input->LastMouseY / 48;
			/*1024
			Xloc = mYX - tX + 7;
			Yloc = mYY - tY + 7;
			*/

			Xloc = mYX - tX + 6;
			Yloc = mYY - tY + 6;

			if (p->InGame->IsBuilding == -1) //Demolish
			{
				CBuilding *TestBuilding;
				int foundbuilding = 0;
				for (int j = 0; j < 3; j++)
				{
					for (int k = 0; k < 3; k++)
					{
						if (foundbuilding == 0) 
						{
							TestBuilding = p->Build->findBuildingbyLocation(Xloc + j, Yloc + k);
							if (TestBuilding) foundbuilding = 1;
						}
					}
				}


				if (foundbuilding == 1)
				{
					if (p->Tick > DemolishTimer)
					{
						sCMDemolish d_packet;
 						d_packet.id = TestBuilding->id;
						p->Winsock->SendData(cmDemolish,(char *)&d_packet,sizeof(d_packet));
						NeedRelease = 1;

						if (p->Tick - DemolishTimer < 1000)
							DemolishTimer = p->Tick + 3000;
						else
							DemolishTimer = p->Tick;
					}
				}
			}
			else
			{
				if (p->InGame->Cash < 500000 && p->Player[p->Winsock->MyIndex]->isAdmin != 2)
					return;
				if (p->InGame->IsBuilding == 0 && p->InGame->Cash < 2000000)
				{
					return;
				}
				
				if (p->Collision->CheckBuildingCollision(Xloc, Yloc) == 0 || p->Player[p->Winsock->MyIndex]->isAdmin == 2)
				{
					if (p->Build->inRange() == 1)
					{
						if (Xloc > 0 && Xloc < 510 && Yloc > 0 && Yloc < 510)
						{
							sCMBuild b_packet;
							b_packet.x = Xloc;
							b_packet.y = Yloc;
								
							b_packet.type = (unsigned char)p->InGame->IsBuilding;

							p->Winsock->SendData(cmBuild,(char *)&b_packet,sizeof(b_packet));

							p->InGame->IsBuilding = 0;
						}
					}
				}

				NeedRelease = 1;
				return;
			}
		}
		if (p->InGame->ShowBuildMenu)
		{
			p->InGame->ShowBuildMenu = 0;
			if (X > p->Draw->BuildMenuX-26 && X < p->Draw->BuildMenuX+180 && Y < p->Draw->BuildMenuY+16)
			{
				p->Draw->PlayerOffsetX = (p->Draw->MaxMapX / 2) - 24;
				p->Draw->PlayerOffsetY = (p->Draw->MaxMapY / 2) - 24;
				int Ym = p->Draw->BuildMenuY+16;
				if (Y > Ym -16 && Y < Ym)
						{
							p->InGame->IsBuilding = -1;
							NeedRelease = 1;
							return;
						}
				for (int j = 0; j < 26; j++)
				{
					if (p->InGame->CanBuild[j] || p->Player[p->Winsock->MyIndex]->isAdmin == 2)
					{
						Ym -= 16;
						if (Y > Ym -16&& Y < Ym)
						{
							p->InGame->IsBuilding = j+1;
							if (p->InGame->IsBuilding == 1 && p->Options->newbietips == 1) p->InGame->NewbieTip = "Stand on a hospital to restore your health! Hospitals are expensive to maintain so if you are running low on money, demolish it.";
							if (p->InGame->IsBuilding == 2 && p->Options->newbietips == 1) p->InGame->NewbieTip = "Houses are required for your city population. For every 2 non-housing buildings you construct, you will need 1 house.";
							if (p->InGame->IsBuilding > 2 && p->Options->newbietips == 1)
							{
								if (p->InGame->IsBuilding % 2)
									 p->InGame->NewbieTip = "After research is complete, you will be able to build a factory which will produce that item.";
								else
									 p->InGame->NewbieTip = "Factories produce items. Press 'u' while on top of the item to pick it up. Press 'd' to drop the item.";
							}
							NeedRelease = 1;
							return;
						}	
					}
				}
			}
		}
		if (p->Player[p->Winsock->MyIndex]->isAdmin == 2)
		{
			if (X > (p->Draw->MaxMapX + 110) && X < (p->Draw->MaxMapX + 110 + 42) && Y > 460 && Y < 479)
			{
				p->Admin->ShowAdminDlg();
			}
		}

		// Inventory
		if ((X > (p->Draw->MaxMapX + 7)) && (X < (p->Draw->MaxMapX + 109)) && (Y > 267) && (Y < 404))
		{
			p->Inventory->SelectedItemType = ((X - p->Draw->MaxMapX - 7) / 35);
			p->Inventory->SelectedItemType = (p->Inventory->SelectedItemType + (((Y - 267) / 35) * 3));
		}

		if (X > p->Draw->MaxMapX) //Clickable area of the panel
		{
			if ((X > (p->Draw->MaxMapX+145)) && (X < (p->Draw->MaxMapX+190)) && (Y < 420)) //Main Buttons
			{
				if (Y > 268 && Y < 288) //Staff button
				{
					if (p->Player[p->Winsock->MyIndex]->isMayor)
					{
						p->Winsock->SendData(cmComms, "|");
						p->Personnel->ShowPersonnelDialog();
						NeedRelease = 1;
					}
				}
				if (Y > 290 && Y < 310) //Map button
				{
					p->InGame->ShowMap = 1 - p->InGame->ShowMap;
					p->Player[p->Winsock->MyIndex]->isMoving = 0;
					if (p->InGame->ShowMap == 1) p->Winsock->SendData(cmMiniMap, " ");
					NeedRelease = 1;
				}
				if (Y > 312 && Y < 332) //Info button
				{
					this->InfoButton();
					NeedRelease = 1;
				}
				if (Y > 334 && Y < 354) //Points button
				{
					PointsButton();
					NeedRelease = 1;
				}
				if (Y > 356 && Y < 376) //Options Button
				{
					p->Options->ShowOptionsDialog();
					NeedRelease = 1;
				}
				if (Y > 378 && Y < 398) //Help Button
				{
					p->Help->ShowHelpDialog();
					NeedRelease = 1;
				}
			}

			if ((Y > 400) && (Y < 422) && (X > (p->Draw->MaxMapX + 126)) && (X < (p->Draw->MaxMapX + 190))) //Build Button
			{
				p->Draw->BuildMenuX = p->Draw->MaxMapX - 180;
				p->Draw->BuildMenuY = 344;
				p->InGame->ShowBuildMenu = 1;
				NeedRelease = 1;
			}

			if ((Y > 576) && (Y < 594) && (X > (p->Draw->MaxMapX + 150)) && (X < (p->Draw->MaxMapX + 192))) //Quit Button
			{
				p->Engine->ThreadMessageQuit();
				NeedRelease = 1;
			}

			if ((X > p->Draw->MaxMapX + 2) && (X < p->Draw->MaxMapX + 2 + 120) && (Y > 224) && (Y < 243))
			{
				if (p->Player[p->Winsock->MyIndex]->isMayor)
				{
					p->InGame->PrintFinanceReport();
				}
			}
		}
	}

	if (mouse_state.rgbButtons[1] & 0X80) //Right Click
	{
		// Inventory
		if ((X > (p->Draw->MaxMapX + 7)) && (X < (p->Draw->MaxMapX + 109)) && (Y > 267) && (Y < 404))
		{
			p->Inventory->SelectedItemType = ((X - p->Draw->MaxMapX - 7) / 35);
			p->Inventory->SelectedItemType = (p->Inventory->SelectedItemType + (((Y - 267) / 35) * 3));

			if (p->Inventory->SelectedItemType == 2) // Med Kit
			{
				//Use Health Pack
				CItem *itm = p->Inventory->findItembyType(2);
				if (itm)
				{
					p->Winsock->SendData(cmMedKit, (char *)&itm->id, sizeof(itm->id));
					this->NeedRelease = 1;
				}
			}

			if (p->Inventory->SelectedItemType == 3) // Bomb
			{
				p->InGame->BombsAreActivated = 1 - p->InGame->BombsAreActivated;
				this->NeedRelease = 1;
			}
		}

		if ((X > p->Draw->MaxMapX + 2) && (X < p->Draw->MaxMapX + 2 + 120) && (Y > 224) && (Y < 243))
		{
			if (p->Player[p->Winsock->MyIndex]->isMayor)
			{
				p->InGame->PrintFinanceReport();
			}
		}

		if (X >= 0 && X <= p->Draw->MaxMapX && Y >= 0 && Y <= p->Draw->MaxMapY)
		{
			p->Draw->PlayerOffsetX = (p->Draw->MaxMapX / 2) - 24;
			p->Draw->PlayerOffsetY = (p->Draw->MaxMapY / 2) - 24;

			int me = p->Winsock->MyIndex;
			int mYX = (int)p->Player[me]->X / 48;
			int mYY = (int)p->Player[me]->Y / 48;
			int tX = p->Input->LastMouseX / 48;
			int tY = p->Input->LastMouseY / 48;
			int Xloc = mYX - tX + 6;
			int Yloc = mYY - tY + 6;

			int foundplayer = 0;
			for (int i = 0; i < MaxPlayers; i++)
			{
				if (foundplayer == 0)
				{
					Rect rct1, rct2;

					rct1.X = (int)(p->Player[me]->X - p->Player[i]->X + p->Draw->PlayerOffsetX);
					rct1.Y = (int)(p->Player[me]->Y - p->Player[i]->Y + p->Draw->PlayerOffsetY);
					rct1.w = 48;
					rct1.h = 48;

					rct2.X = p->Input->LastMouseX;
					rct2.Y = p->Input->LastMouseY;
					rct2.w = 1;
					rct2.h = 1;
					
					if (p->Collision->RectCollision(rct1, rct2))
					{
						foundplayer = 1;
						if (p->Engine->MsgQueue == 0)
						{
							p->Engine->MsgQueue = 1;
							char packet[2];
							packet[0] = i;
							packet[1] = 0;
							p->Winsock->SendData(cmClickPlayer, packet, 1);
						}
					}
				}
			}

			CBuilding *TestBuilding;
			int foundbuilding = 0;

			for (int j = 0; j < 3; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					if (foundbuilding == 0) 
					{
						TestBuilding = p->Build->findBuildingbyLocation(Xloc + j, Yloc + k);
						if (TestBuilding) foundbuilding = 1;
					}
				}
			}

			if (foundbuilding > 0 || foundplayer > 0)
			{
				if (foundbuilding > 0)
				{
					if (p->Engine->MsgQueue == 0)
					{
						p->Engine->MsgQueue = 1;
						char packet[2];
						packet[0] = TestBuilding->City;
						packet[1] = 0;
						p->Winsock->SendData(cmRightClickCity, packet, sizeof(packet));
					}
				}
			}
			else
			{
				if (p->Player[p->Winsock->MyIndex]->isMayor)
				{
					p->InGame->IsBuilding = 0;
					p->InGame->ShowBuildMenu = 1;
					p->Draw->BuildMenuX = X;
					p->Draw->BuildMenuY = Y;
					if(X < 16)
						p->Draw->BuildMenuX = 16;
					if(Y > 660)
						p->Draw->BuildMenuY = 660;
				}
			}
		}
	}
	
    oMouseState.rgbButtons[0] = mouse_state.rgbButtons[0];
    oMouseState.rgbButtons[1] = mouse_state.rgbButtons[1];
}

void CInput::StartDInput()
{
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

void CInput::Cycle()
{
	if (!m_Keyboard) return;
	if (!m_Mouse) return;
	if (!HasFocus) return;

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

	if ((Mouse.x != oMouse.x) || (Mouse.y != oMouse.y)) 
	{
		MouseMove(mouse_state, Mouse.x, Mouse.y, buffer);
		oMouse.x = Mouse.x;
		oMouse.y = Mouse.y;
	}

	if ((mouse_state.rgbButtons[0] & 128) || (mouse_state.rgbButtons[1] & 128))
    {
		MouseDown(mouse_state, Mouse.x, Mouse.y, buffer);
	}

	if ((!(mouse_state.rgbButtons[0] & 128) && (oMouseState.rgbButtons[0] & 128)) || (!(mouse_state.rgbButtons[1] & 128) && (oMouseState.rgbButtons[1] & 128))) {
		MouseUp(mouse_state, Mouse.x, Mouse.y, buffer);
	}

	oMouseState = mouse_state;

	ProcessKeys(buffer);
}

void CInput::InfoButton()
{
	p->InGame->PrintWhoData();

	p->Winsock->SendData(cmRequestInfo, " ");
}

void CInput::PointsButton()
{
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