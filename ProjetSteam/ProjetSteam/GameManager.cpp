#include "GameManager.h"

GameManager::GameManager()
{
	bRunning = true;
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
	while (bRunning)
	{
		if (pSdlManager != nullptr)
		{
			bRunning = pSdlManager->SDLUpdate();
		}

		if (pSteamManager != nullptr)
		{
			pSteamManager->SteamUpdate();
		}
	}
}