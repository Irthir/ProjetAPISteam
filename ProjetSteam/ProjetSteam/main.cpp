#include<windows.h>
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
			gameManager.setSteamManager(&steamApiManager);

			gameManager.Update();
		}
		else
		{
			printf_s("Erreur lors du chargement de l'API Steam");
		}

	}
	else
	{
		printf_s("Erreur lors du chargement de la SDL.");
	}

	Sleep(3000);


	return 0;
}

