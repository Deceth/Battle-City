#ifndef _CPROCESS
#define _CPROCESS

#include "CGame.h"
#include "NetMessages.h"

class CGame;

class CProcess
{
public:
	CProcess(CGame *game);
	~CProcess();

	int ProcessData(char *TheData);
	void ProcessEvent(int Event);

	void ProcessLogin(unsigned char Index, unsigned char playerType);
	void ProcessClearPlayer(unsigned char Index);
	void ProcessChatMessage(char *TheData, char global);
	void ProcessChatCommand(int Index, int message);
	void ProcessEnterGame(sSMStateGame *game);
	void ProcessWarp(sSMStateGame *game);
	void ProcessError(int error);
	void ProcessNewBuilding(sSMBuild *, int sound = 1);
	void ProcessUpdate(sSMUpdate *data);
	void ProcessPlayerData(sSMPlayer *player);
	void ProcessCanBuild(char TheData[255]);
	void ProcessComms(char TheData[255]);
	void ProcessRejected();
	void ProcessInterview();
	void ProcessMayorHire(int Index);
	void ProcessAddItem(sSMItem *item);
	void ProcessRemoveItem(sSMItem *item);
	void ProcessItemCount(sSMItemCount *count);
	void ProcessJoinData(sSMJoinData *join);
	void ProcessPickUp(sCMItem *item);
	void ProcessDropped(sCMItem *item);
	void ProcessPop(sSMPop *b);
	void ProcessDeath(int Index, char deathType, char theCity);
	void ProcessShot(sSMShot *shotsfired);
	void ProcessDestroyCity(char City);
	void ProcessRemoveBuilding(sSMBuild *build);
	void ProcessOrbed(sSMOrbedCity *orbed);
	void ProcessPointsUpdate(sSMPoints *pts);
	void ProcessMayorUpdate(sSMMayorUpdate *pts);
	void ProcessItemLife(sSMItemLife *itm);
	void ProcessMedKit();
	void ProcessCloak(int Index);
	void ProcessExplosion(sSMExplode *bomb);
	void ProcessMayorInInterview();
	void ProcessInterviewCancel();
	void ProcessFired();
	void ProcessKicked();
	void ProcessAdmin(sSMAdmin *admin);
	void ProcessSectorSent(sCMSector *sector);
	void ProcessEditAccount(char TheData[255]);
	void ProcessInfoButton(sSMInfoButton *selectedcity);
	void ProcessMayorChanged();
	void ProcessRightClickCity(sSMRightClickCity *cityinfo);
	void ProcessRespawn(unsigned char Index);
	void ProcessPong();
	void ProcessAdminEdit(sCMAdminEdit *adminedit);
	void ProcessClickPlayer(sSMClickPlayer *clickplayer);
	void ProcessFinance(sSMFinance *finance);
	void ProcessWhisper(char *TheData);
	void ProcessGlobal(char *TheData);
	void ProcessUnderAttack();
	void ProcessAutoBuild(sSMAutoBuild* response);

private:
	CGame *p;
protected:

};

#endif