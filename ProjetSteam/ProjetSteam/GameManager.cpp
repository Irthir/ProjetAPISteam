#include "GameManager.h"

GameManager::GameManager()
{
	bRunning = true;
	pSdlManager = nullptr;
}

void GameManager::setSDLManager(SDLManager* pSdlManager)
{
	this->pSdlManager = pSdlManager;
}

void GameManager::Update()
{
	while (bRunning)
	{
		if (pSdlManager != nullptr)
		{
			bRunning = pSdlManager->SDLUpdate();
		}
	}
}