#include "CPlayer.h"

CPlayer::CPlayer(CGame *game, int initid)
{
	p = game;

	isMoving = 0;
	isDead = false;
	timeMove = 0;
	isTurning = 0;
	timeTurn = 0;
	Direction = 0;
	X = 0;
	Y = 0;
	SectorX = 0;
	SectorY = 0;

	City = 0;
	CityX = 0;
	CityY = 0;

	Points = 0;
	MonthlyPoints = 0;
	Orbs = 0;
	Deaths = 0;
	Assists = 0;
	isMayor = 0;
	isShooting = 0;
	isFrozen = 0;
	isAdmin = 1;
	isLagging = 0;
	isInGame = 0;
	timeFrozen = 0;

	Name.clear();
	NameString.clear();
	TownString.clear();
	Town.clear();

	lastUpdate = 0;
	id = initid;
}

void CPlayer::ClearPlayer()
{
	isMoving = 0;
	isDead = false;
	timeMove = 0;
	isTurning = 0;
	timeTurn = 0;
	Direction = 0;
	X = 0;
	Y = 0;
	SectorX = 0;
	SectorY = 0;

	City = 0;
	CityX = 0;
	CityY = 0;

	Points = 0;
	MonthlyPoints = 0;
	Orbs = 0;
	Deaths = 0;
	Assists = 0;
	isMayor = 0;
	isShooting = 0;
	isFrozen = 0;
	isAdmin = 1;
	isLagging = 0;
	isInGame = 0;
	timeFrozen = 0;

	Name.clear();
	NameString.clear();
	TownString.clear();
	Town.clear();

	lastUpdate = 0;
}

void CPlayer::InGameClear()
{
	isMoving = 0;
	isDead = false;
	timeMove = 0;
	isTurning = 0;
	timeTurn = 0;
	Direction = 0;
	X = 0;
	Y = 0;
	SectorX = 0;
	SectorY = 0;

	City = 0;
	CityX = 0;
	CityY = 0;

	isMayor = 0;
	isShooting = 0;
	isFrozen = 0;
	isLagging = 0;
	isInGame = 0;
	timeFrozen = 0;

	lastUpdate = 0;
}

void CPlayer::Cycle()
{
	float MoveFactor = 0.38f;
	if (this->isAdmin == 2) MoveFactor = 1.0f;
	if (this->isInGame)
	{
		if (id != p->Winsock->MyIndex && p->Tick > (this->lastUpdate + 15000))
		{
			this->X = 0;
			this->Y = 0;
		}
		else if (id != p->Winsock->MyIndex && p->Tick > (this->lastUpdate + 3000))
		{
			this->isLagging = 1;
			this->isMoving = 0;
		}
		else
		{
			this->isLagging = 0;
		}
		if (this->isFrozen)
		{
			isMoving = 0;
			if (p->Tick > this->timeFrozen)
			{
				this->isFrozen = 0;
			}
			else
				return;
		}

		int me = p->Winsock->MyIndex;
		float velX = 0;
		float velY = 0;

		if (this->isTurning)
			if (p->Tick > this->timeTurn)
			{
				this->Direction += this->isTurning;
				if (this->Direction < 0) //Prevent the Direction from going negative
					this->Direction = 31;
				if (this->Direction > 31) //Prevent the Direction from going above 31
					this->Direction = 0;

				this->timeTurn = p->Tick + 50; //Set the time for when player last turned
			}

		short HasSectorX = short(X/48) / SectorSize;
		short HasSectorY = short(Y/48) / SectorSize;
		if (HasSectorX >= 0 && HasSectorX <= MaxSectors && HasSectorY >= 0 && HasSectorY <= MaxSectors)
			if(isMoving && (p->InGame->HasSector[HasSectorX][HasSectorY] == 1 || id != p->Winsock->MyIndex))
			{
				int movetimes = 0;

				// must reverse order for math purposes
				float fDir = (float)-Direction + 32; // trig goes counter-clockwise and bc isTurning goes clockwise.
				
				velX = (float)((sin((float)(fDir)/16*3.14)*isMoving)) * (p->TimePassed * MoveFactor); //calculate their speed
				if (velX > 20) velX = 20;
				if (velX < -20) velX = -20;
				X += velX; //update X position

				switch(p->Collision->CheckPlayerCollision(this->id))
				{
				case 0:
					//No collision
					break;
				case 2:
					//Blocking collision
					do
					{
						if (movetimes > 1000) break;
						X += (float)((sin((float)(fDir+16)/16*3.14)*isMoving));
						movetimes += 1;
					} 
					while (p->Collision->CheckPlayerCollision(this->id) == 2);
					break;
				case 101: //Mine
					this->HitMine();
					break;
				case 103: //DFG
					this->HitDFG();
					break;
				case 200: //Left map edge
					X = 0;
					break;
				case 201: //Right map edge
					X = (512-1) * 48;
					break;
				}
				
				movetimes = 0;
				velY = (float)((cos((float)(fDir)/16*3.14)*isMoving)) * (p->TimePassed * MoveFactor);
				if (velY > 20) velY = 20;
				if (velY < -20) velY = -20;
				Y += velY;

				switch(p->Collision->CheckPlayerCollision(this->id))
				{
				case 0:
					//No collision
					break;
				case 101: //Mine
					this->HitMine();
					break;
				case 103: //DFG
					this->HitDFG();
					break;
				case 2:
					//Blocking collision
					do
					{
						if (movetimes > 1000) break;
						Y += (float)((cos((float)(fDir+16)/16*3.14)*isMoving)); //isMoving them back slowlY from the opposite Direction theY came from until there is no collision
						movetimes += 1;
					} 
					while (p->Collision->CheckPlayerCollision(this->id) == 2);
					break;
				case 202: //Top map edge
					Y = 0;
					break;
				case 203: //Bottom map edge
					Y = (512-1)*48;
					break;
				}
			}
			else
			{
				switch(p->Collision->CheckPlayerCollision(this->id))
				{
				case 2:
					p->Player[id]->RelocatePlayer();
					break;
				}
			}
	}
}

CPlayer::~CPlayer()
{

}

void CPlayer::RelocatePlayer()
{
	float fDir = 0;
	int tempdir = 0;
	int isMovingtimesforward = 0;
	int isMovingtimesback = 0;
	int isMovingtimesleft = 0;
	int isMovingtimesright = 0;

	int startX = (int)X;
	int startY = (int)Y;

	int finalXforward = 0;
	int finalYforward = 0;
	int finalXback = 0;
	int finalYback = 0;
	int finalXleft = 0;
	int finalYleft = 0;
	int finalXright = 0;
	int finalYright = 0;

	//Check closest tiles first
	int mYX = (int)X / 48;
	int mYY = (int)Y / 48;
	
	X = (float)(startX); 
	Y = (float)(startY);
	X = (float)(mYX - 1) * 48;
	Y = (float)(mYY) * 48;
	if (p->Collision->CheckPlayerCollision(this->id) == 0) return;

	X = (float)(startX);
	Y = (float)(startY);
	X = (float)(mYX + 1) * 48;
	Y = (float)(mYY) * 48;
	if (p->Collision->CheckPlayerCollision(this->id) == 0) return;

	X = (float)(startX);
	Y = (float)(startY);
	X = (float)(mYX) * 48;
	Y = (float)(mYY - 1) * 48;
	if (p->Collision->CheckPlayerCollision(this->id) == 0) return;

	X = (float)(startX);
	Y = (float)(startY);
	X = (float)(mYX) * 48;
	Y = (float)(mYY + 1) * 48;
	if (p->Collision->CheckPlayerCollision(this->id) == 0) return;

	X = (float)(startX);
	Y = (float)(startY);
	X = (float)(mYX - 1) * 48;
	Y = (float)(mYY - 1) * 48;
	if (p->Collision->CheckPlayerCollision(this->id) == 0) return;

	X = (float)(startX);
	Y = (float)(startY);
	X = (float)(mYX - 1) * 48;
	Y = (float)(mYY + 1) * 48;
	if (p->Collision->CheckPlayerCollision(this->id) == 0) return;

	X = (float)(startX);
	Y = (float)(startY);
	X = (float)(mYX + 1) * 48;
	Y = (float)(mYY - 1) * 48;
	if (p->Collision->CheckPlayerCollision(this->id) == 0) return;

	X = (float)(startX);
	Y = (float)(startY);
	X = (float)(mYX + 1) * 48;
	Y = (float)(mYY + 1) * 48;
	if (p->Collision->CheckPlayerCollision(this->id) == 0) return;

	X = (float)startX;
	Y = (float)startY;

	//Up
	fDir = 0;

	do
	{
		if (isMovingtimesforward > 1000) break;
		Y += (float)(cos((float)(fDir+16)/16*3.14)*-1);
		isMovingtimesforward += 1;
	} 
	while (p->Collision->CheckPlayerCollision(this->id) == 2);

	finalXforward = (int)X;
	finalYforward = (int)Y;

	//Back
	fDir = 24;

	X = (float)startX;
	Y = (float)startY;

	do
	{
		if (isMovingtimesback > 1000) break;
		Y += (float)(cos((float)(fDir+16)/16*3.14)*-1);
		isMovingtimesback += 1;
	} 
	while (p->Collision->CheckPlayerCollision(this->id) == 2);

	finalXback = (int)X;
	finalYback = (int)Y;

	//Left
	tempdir = 36;
	if (tempdir < 0) tempdir = 47 - abs(tempdir);
	fDir = (float)-tempdir + 32;

	X = (float)startX;
	Y = (float)startY;

	do
	{
		if (isMovingtimesleft > 1000) break;
		X += (float)(sin((float)(fDir+16)/16*3.14)*-1);
		isMovingtimesleft += 1;
	} 
	while (p->Collision->CheckPlayerCollision(this->id) == 2);

	finalXleft = (int)X;
	finalYleft = (int)Y;

	//Right
	tempdir = 12;
	if (tempdir > 47) tempdir = tempdir - 47;
	fDir = (float)-tempdir + 32;

	X = (float)startX;
	Y = (float)startY;

	do
	{
		if (isMovingtimesright > 1000) break;
		X += (float)(sin((float)(fDir+16)/16*3.14)*-1);
		isMovingtimesright += 1;
	} 
	while (p->Collision->CheckPlayerCollision(this->id) == 2);

	finalXright = (int)X;
	finalYright = (int)Y;

	if ((isMovingtimesforward <= isMovingtimesback) && (isMovingtimesforward <= isMovingtimesleft) && (isMovingtimesforward <= isMovingtimesright))
	{
		X = (float)finalXforward;
		Y = (float)finalYforward;
	}
	else if ((isMovingtimesback <= isMovingtimesforward) && (isMovingtimesback <= isMovingtimesleft) && (isMovingtimesback <= isMovingtimesright))
	{
		X = (float)finalXback;
		Y = (float)finalYback;
	}
	else if ((isMovingtimesleft <= isMovingtimesforward) && (isMovingtimesleft <= isMovingtimesback) && (isMovingtimesleft <= isMovingtimesright))
	{
		X = (float)finalXleft;
		Y = (float)finalYleft;
	}
	else if ((isMovingtimesright <= isMovingtimesforward) && (isMovingtimesright <= isMovingtimesback) && (isMovingtimesright <= isMovingtimesleft))
	{
		X = (float)finalXright;
		Y = (float)finalYright;
	}
}

void CPlayer::SetHP(int HP)
{
	this->HP = HP;
	this->refHP1 = (HP^2-31)*2;
	this->refHP2 = -(HP);
}

void CPlayer::HitMine()
{
	if (p->Player[p->Winsock->MyIndex]->isDead == true) return;
	int me = p->Winsock->MyIndex;
	if (me == this->id)
	{
		CItem *item = p->Item->findItembyLocationAndType((int)(p->Player[p->Winsock->MyIndex]->X + 24) / 48, (int)(p->Player[p->Winsock->MyIndex]->Y + 24) / 48, 4);
		if (item)
		{
			sCMItem ii;
			ii.id = item->id;
			ii.active = item->active;
			ii.type = item->Type;
			p->Winsock->SendData(cmHitObject,(char *)&ii,sizeof(ii));

			// If player HP is under 20, then mine should destroy player tank
			if (p->Player[me]->HP < 20)
			{
				char packet[2];
				packet[0] = item->City;
				packet[1] = 0;
				p->Player[p->Winsock->MyIndex]->isDead = true;
				p->InGame->timeDeath = p->Tick;
				p->Winsock->SendData(cmDeath, packet, 1);
				p->Sound->PlayWav(sDie, -1);
			}
			else
			{
				// Subtract 19 from total HP when hitting a mine. Player can suurvive 2 mines with these settings
				p->Player[me]->SetHP(p->Player[me]->HP - 19);
				p->Explode->newExplosion(item->X * 48, item->Y * 48, 1);
				item->active = 0;
			}

			p->Sound->PlayWav(sExplode, -1);
		}
	}
	else
	{
		if ((abs(p->Player[this->id]->X - p->Player[me]->X) < 1000) && (abs(p->Player[this->id]->Y - p->Player[me]->Y) < 1000))
		{
			p->Sound->Play3dSound(p->Sound->s_eXplode, 100, p->Player[this->id]->X, p->Player[this->id]->Y);
		}
	}
}

void CPlayer::HitDFG()
{
	if (id == p->Winsock->MyIndex)
	{
		CItem *item = p->Item->findItembyLocationAndType((int)(p->Player[p->Winsock->MyIndex]->X + 24) / 48, (int)(p->Player[p->Winsock->MyIndex]->Y + 24) / 48, 7);
		if (item)
		{
			item->active = 0;
			this->isFrozen = 1;
			this->timeFrozen = p->Tick + 5000;
		}
	}
}

void CPlayer::GenerateNameString()
{
	if (isAdmin == 2)
		NameString = "Admin";
	else
		NameString = p->InGame->ReturnRank(Points);

	NameString += " ";
	NameString += Name;

	TownString = "(";
	if (isMayor) TownString += "Mayor of ";
	TownString += CityList[City];
	TownString += ")";
}