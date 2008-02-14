#include "CSend.h"

CSend::CSend(CGame *game)
{
	p = game;
}

CSend::~CSend()
{

}

void CSend::SendLogin(int NewAccount)
{
	if (NewAccount == 0)
	{
		sCMLogin LoginData;
		memset(&LoginData, 0, sizeof(LoginData));
		strcpy(LoginData.User, p->Login->user);
		strcpy(LoginData.Pass, p->Login->pass);

		p->Winsock->SendData(cmLogin, (char *)&LoginData, sizeof(LoginData));
	}
	else
	{
		sCMNewAccount NewAccount;
		memset(&NewAccount, 0, sizeof(NewAccount));
		strcpy(NewAccount.Pass, p->NewAccount->pass);
		strcpy(NewAccount.User, p->NewAccount->user);
		strcpy(NewAccount.Town, p->NewAccount->town);
		strcpy(NewAccount.State, p->NewAccount->state);
		strcpy(NewAccount.Email, p->NewAccount->email);
		strcpy(NewAccount.FullName, p->NewAccount->fullname);

		p->Winsock->SendData(cmNewAccount, (char *)&NewAccount, sizeof(NewAccount));
	}
}

void CSend::SendVersion()
{
	sCMVersion vers;
	memset(vers.Version, 0, 10);
	memset(vers.UniqID, 0, 50);

	strcpy(vers.Version, VERSION);
	string UniqID = p->ReturnUniqID();
	strcpy(vers.UniqID, UniqID.c_str());
	p->Winsock->SendData(cmVersion, (char *)&vers, sizeof(vers));
}

void CSend::SendAccountUpdate()
{
	sCMNewAccount NewAccount;
	memset(&NewAccount, 0, sizeof(NewAccount));
	strcpy(NewAccount.Pass, p->NewAccount->pass);
	strcpy(NewAccount.User, p->NewAccount->user);
	strcpy(NewAccount.Town, p->NewAccount->town);
	strcpy(NewAccount.State, p->NewAccount->state);
	strcpy(NewAccount.Email, p->NewAccount->email);
	strcpy(NewAccount.FullName, p->NewAccount->fullname);

	p->Winsock->SendData(cmAccountUpdate, (char *)&NewAccount, sizeof(NewAccount));
}

void CSend::SendAccountEdit()
{	
		sCMLogin LoginData;
		memset(&LoginData, 0, sizeof(LoginData));
		strcpy(LoginData.User, p->Login->user);
		strcpy(LoginData.Pass, p->Login->pass);

		p->Winsock->SendData(cmEditAccount, (char *)&LoginData, sizeof(LoginData));
}