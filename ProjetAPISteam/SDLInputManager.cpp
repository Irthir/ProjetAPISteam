#include "SDLInputManager.h"

SDLInputManager::SDLInputManager(GameManager* pGameManager)
{
	this->pGameManager = pGameManager;
}

	
bool SDLInputManager::SDLInputUpdate()
{
	bool bRunning = true;
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			bRunning = false;
			break;
		case SDL_MOUSEBUTTONDOWN:
			bRunning = false;
			break;
		case SDL_KEYDOWN:
			SDLKeyInputHandler(SDL_GetKeyName(event.key.keysym.sym));
		break;
		default:
			break;
		}
	}
	return bRunning;
}

void SDLInputManager::SDLKeyInputHandler(const char* sInput)
{
	char cInput = sInput[0];

	switch (cInput)
	{
	case ('O'):
		//Activation de l'overlay si l'utilisateur appuie sur "O".
		pGameManager->pSteamManager->ActivateOverlay();
		break;
	default:
		std::cout << cInput << std::endl;
		break;
	}
}