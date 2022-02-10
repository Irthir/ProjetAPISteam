#include "SteamAPIManager.h"

SteamAPIManager::SteamAPIManager()
{}

bool SteamAPIManager::SteamAPIInit()
{
	if (!SteamAPI_Init())
	{
		//return EXIT_FAILURE;
		printf_s("Erreur lors de l'initialisation de l'api steam.");
		return false;
	}

	if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid))
	{
		//return EXIT_FAILURE;
		printf_s("Erreur lors du rechargement de l'api steam.");
		return false;
	}

	const char* name = SteamFriends()->GetPersonaName();

	printf_s(name);
	return true;
}

void SteamAPIManager::SteamAPIQuit()
{
	// Shutdown the SteamAPI
	SteamAPI_Shutdown();
}