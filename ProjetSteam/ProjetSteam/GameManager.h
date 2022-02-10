#pragma once
#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "SDLManager.h"

class GameManager
{
private:
	SDLManager* pSdlManager;

public:
	bool bRunning;

	//Constructeurs
	GameManager();
	
	//Mutateurs
	void setSDLManager(SDLManager* pSdlManager);
	
	//Fonctions
	void Update();
};

#endif