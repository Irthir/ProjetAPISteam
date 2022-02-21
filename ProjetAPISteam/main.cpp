#include "GameManager.h"
#include "SDLManager.h"
#include "SteamAPIManager.h"
#include "SDLInputManager.h"
#include <Windows.h>

int main(int argc, char* args[])
{
	setlocale(LC_ALL, ""); //Pour mettre les accents dans la console.

	GameManager gameManager; //Gestionnaire du jeu
	SDLManager sdlManager; //Affichage
	SteamAPIManager steamApiManager; //Gestion de l'API Steam

	SDLInputManager sdlInputManager(&gameManager); //Gestion des inputs à revoir dans la structure

	if (steamApiManager.SteamAPIInit())
	{
		gameManager.setSteamManager(&steamApiManager); //Gestion de l'api steam (réseau)
		if (sdlManager.SDLInit())
		{
			gameManager.setSDLManager(&sdlManager); //Gestion de l'affichage

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
