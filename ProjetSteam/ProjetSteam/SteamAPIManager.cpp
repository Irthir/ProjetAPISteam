#include "SteamAPIManager.h"
SteamAPIManager::SteamAPIManager()
//Constructeur
{}

SteamAPIManager::~SteamAPIManager()
//Destructeur
{
	SteamAPIQuit();
}

void SteamAPIManager::OnFriendStatusChanged(PersonaStateChange* pCallback)
{
	if (pCallback->m_bActive)
		printf_s("Steam overlay now active\n");
	else
		printf_s("Steam overlay now inactive\n");
}


bool SteamAPIManager::SteamAPIInit()
//BUT : Initialisation de l'API Steam
{
	//if (SteamAPI_RestartAppIfNecessary(480))
	if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid))
	{
		//return EXIT_FAILURE;
		printf_s("Erreur lors du rechargement de l'api steam.");
		return false;
	}

	// Initialize SteamAPI, if this fails we bail out since we depend on Steam for lots of stuff.
	// You don't necessarily have to though if you write your code to check whether all the Steam
	// interfaces are NULL before using them and provide alternate paths when they are unavailable.
	//
	// This will also load the in-game steam overlay dll into your process.  That dll is normally
	// injected by steam when it launches games, but by calling this you cause it to always load,
	// even when not launched via steam.
	if (!SteamAPI_Init())
	{
		printf_s("Erreur lors de l'initialisation de l'api steam.");
		return false;
	}

	// set our debug handler
	//SteamClient()->SetWarningMessageHook(&SteamAPIDebugTextHook);

	// Ensure that the user has logged into Steam. This will always return true if the game is launched
	// from Steam, but if Steam is at the login prompt when you run your game from the debugger, it
	// will return false.
	if (!SteamUser()->BLoggedOn())
	{
		printf_s("Erreur l'utilisateur doit être connecté pour jouer au jeu.\n");
		return false;
	}

	//Test du Steamworks Interfaces pour afficher le nom de l'utilisateur.
	const char* name = SteamFriends()->GetPersonaName();
	printf_s(name);

	return true;
}

void SteamAPIManager::SteamAPIQuit()
//BUT : Fermeture de l'API Steam
{
	// Shutdown the SteamAPI
	SteamAPI_Shutdown();

}


void SteamAPIManager::SteamUpdate()
{
	SteamAPI_RunCallbacks();
}

void SteamAPIManager::ActivateOverlay()
{
	printf_s("Activate overlay\n");
	SteamFriends()->ActivateGameOverlay("friends");
}