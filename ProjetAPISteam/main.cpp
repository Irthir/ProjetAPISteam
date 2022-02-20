#include "GameManager.h"
#include "SDLManager.h"
#include "SteamAPIManager.h"
#include "SDLInputManager.h"

int main(int argc, char* args[])
{
	setlocale(LC_ALL, ""); //Pour mettre les accents dans la console.

	GameManager gameManager;
	SDLManager sdlManager;
	SteamAPIManager steamApiManager;
	SDLInputManager sdlInputManager(&gameManager);

	if (steamApiManager.SteamAPIInit())
	{
		gameManager.setSteamManager(&steamApiManager);
		if (sdlManager.SDLInit())
		{
			gameManager.setSDLManager(&sdlManager);

			while (sdlInputManager.SDLInputUpdate())
			{
				gameManager.Update();
			}
			
			sdlManager.SDLQuit();
		}
		else
		{
			printf_s("Erreur lors du chargement de la SDL.");
		}
		steamApiManager.SteamAPIQuit();
	}
	else
	{
		printf_s("Erreur lors du chargement de steam.");
	}

	return 0;
}
