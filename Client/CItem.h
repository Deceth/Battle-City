#ifndef __CITEM__
#define __CITEM__

#include "CGame.h"

class CGame;
class CBullet;

class CItem {

	public:
		CGame *p;
		
		CItem(int X, int Y, int Type, int City, unsigned short id, int active, CGame *game);
		~CItem();
		void targetNearestEnemy();
		void verifyBulletExists();

		int X;
		int Y;
		int active;
		unsigned short id;
		int Life;
		int Type;
		int City;

		float Angle;
		char status;
		char dmg;
		float lastturn;
		int target;
		int bulletDamage;
		CBullet *bullet;

		int Animation;
		float Animationtick;

		CItem *next;
		CItem *prev;
};

class CItemList {

	public:
		CItem *itemListHead;
		CGame *p;

		CItem *findItem(unsigned short id);

		CItem *newItem(int X, int Y, int Type, int City, unsigned short id, int active);
		CItem *delItem(CItem *del);
		CItem *findItembyLocation(int X, int Y);
		CItem *findItembyLocationAndType(int X, int Y, int Type, bool activeOnly);
		void deleteItemsByCity(char theCity);

		CItemList(CGame *game);
		~CItemList();
		void Cycle();
};

class CInventory {

	public:
		CItem *itemListHead;

		CItem *findItem(unsigned short id);

		CItem *newItem(int X, int Y, int Type, int City, unsigned short id, int active );
		CItem *delItem(CItem *del);
		CItem *findItembyType(int Type);

		int SelectedItemType;

		void ItemCheck();
		void Drop();
		void triggerItem(int type);

		CInventory(CGame *game);
		~CInventory();

		void Cycle();
		CGame *p;
};

#endif