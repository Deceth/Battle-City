#include "CMap.h"

/***************************************************************
 * Constructor
 *
 * @param Server
 **************************************************************/
CMap::CMap(CServer *Server) {
	this->p = Server;
}

/***************************************************************
 * Destructor
 *
 **************************************************************/
CMap::~CMap() {
}

/***************************************************************
 * Function:	LoadMap
 *
 **************************************************************/
void CMap::LoadMap() {
	FILE *file;

	// For each row,
	for (int i = 0; i < 512; i++) {

		// For each column,
		for (int j = 0; j < 512; j++) {

			// Set the square to 0
			this->map[i][j] = 0;
		}
	}

	// If the map file is missing,
	if (this->p->Exists("map.dat") == 0) {

		// Error and return
		this->p->running = 0;
		cout << "Error:  map.dat is missing!" << endl;
		return;
	}

	// Open and read the map file
	file = fopen("map.dat","r");
	if (file) {
		fread(this->map,512,512,file);
		this->CalculateTiles();
	}

	fclose(file);
}

/***************************************************************
 * Function:	CalculateTiles
 *
 **************************************************************/
void CMap::CalculateTiles() {
	int citIndex = 63;

	// For each row,
	for (int j = 0; j < 512; j++) {

		// For each column,
		for (int i = 0; i < 512; i++) {

			// If the map says to place a CC,
			if (this->map[i][j] == 3) {

				// Place the CC
				p->City[citIndex]->x = i * 48;
				p->City[citIndex]->y = j * 48;
				p->Build->newBuilding(i, j, 0, -1, 0);
				citIndex--;
			}
		}
	}
}
