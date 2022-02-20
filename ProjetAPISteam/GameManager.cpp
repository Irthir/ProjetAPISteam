#include "GameManager.h"

GameManager::GameManager()
{
	pSdlManager = nullptr;
	pSteamManager = nullptr;
}

void GameManager::setSDLManager(SDLManager* pSdlManager)
{
	this->pSdlManager = pSdlManager;
}

void GameManager::setSteamManager(SteamAPIManager* pSteamManager)
{
	this->pSteamManager = pSteamManager;
}

void GameManager::Update()
{
	if (pSdlManager != nullptr)
	{
		pSdlManager->SDLUpdate();
	}

	if (pSteamManager != nullptr)
	{
		pSteamManager->SteamUpdate();
	}
}