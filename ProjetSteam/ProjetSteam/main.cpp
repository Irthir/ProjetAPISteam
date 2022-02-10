#include "GameManager.h"
#include "SDLManager.h"
#include "SteamAPIManager.h"

int main( int argc, char* args[] )
{
	GameManager gameManager;
	SDLManager sdlManager;
	SteamAPIManager steamApiManager;

	if (sdlManager.SDLInit())
	{
		gameManager.setSDLManager(&sdlManager);

		if (steamApiManager.SteamAPIInit())
		{
			gameManager.Update();

			steamApiManager.SteamAPIQuit();
		}

		sdlManager.SDLQuit();
	}
	else
	{
		printf_s("Erreur lors du chargement de la SDL.");
	}

	return 0;
}

