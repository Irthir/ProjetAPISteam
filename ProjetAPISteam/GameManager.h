#pragma once
#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "SDLManager.h"
#include "SteamAPIManager.h"

class GameManager
{
private:
	SDLManager* pSdlManager;

public:
	SteamAPIManager* pSteamManager;

	//Constructeurs
	GameManager();

	//Mutateurs
	void setSDLManager(SDLManager* pSdlManager);
	void setSteamManager(SteamAPIManager* pSteamManager);

	//Fonctions
	void Update();
};

#endif