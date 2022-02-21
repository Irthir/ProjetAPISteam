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
			printf_s("CLICK !\n");
			pGameManager->pSteamManager->SendMessage();
			break;
		case SDL_KEYDOWN:
			SDLKeyInputHandler(SDL_GetKeyName(event.key.keysym.sym), &bRunning);
		break;
		default:
			break;
		}
	}
	return bRunning;
}

void SDLInputManager::SDLKeyInputHandler(const char* sInput, bool* bRunning)
{
	char cInput = sInput[0];

	switch (cInput)
	{
	case ('O'):
		//Activation de l'overlay si l'utilisateur appuie sur "O".
		pGameManager->pSteamManager->ActivateOverlay();
		break;
	case ('Q'):
		//Fermeture du jeu si l'utilisateur appuie sur "Q".
		printf_s("Fermeture du jeu !\n");
		*bRunning = false;
		break;
	}
}