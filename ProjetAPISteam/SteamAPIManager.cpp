#include "SteamAPIManager.h"

SteamAPIManager::SteamAPIManager()
//Constructeur
{}

SteamAPIManager::~SteamAPIManager()
//Destructeur
{
	SteamAPIQuit();
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
	printf_s("\n");

	//Enable listening for friends message ; ! NEED TO BE DONE AFTER INITIALIZATION !
	SteamFriends()->SetListenForFriendsMessages(true);

#ifdef STEAMMANUALCALLBACK
	SteamAPI_ManualDispatch_Init();

	HSteamPipe hSteamPipe = SteamAPI_GetHSteamPipe();
	m_GameOverlayActivated.Set(hSteamPipe, this, &SteamAPIManager::OnGameOverlayActivated);
	m_GameConnectedFriendChatMsg.Set(hSteamPipe, this, &SteamAPIManager::OnMessageReceived);
#endif

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
#ifndef STEAMMANUALCALLBACK
	SteamAPI_RunCallbacks();
#else
	SteamManualCallback();
#endif
}

void SteamAPIManager::ActivateOverlay()
{
	printf_s("Activation de l'overlay Steam\n");
	SteamFriends()->ActivateGameOverlay("Friends");

}

#ifndef STEAMMANUALCALLBACK

void SteamAPIManager::OnGameOverlayActivated(GameOverlayActivated_t* pCallback)
{

	if (pCallback->m_bActive)
		printf_s("\nL'overlay Steam est à présent active !\n");
	else
		printf_s("\nL'overlay Steam est à présent inactive !\n");
}

//Will intercept the message and reply if valid
void SteamAPIManager::OnMessageReceived(GameConnectedFriendChatMsg_t* pCallBack)
{
	EChatEntryType entry = k_EChatEntryTypeInvalid;
	char* tmp = new char[4096];
	SteamFriends()->GetFriendMessage(pCallBack->m_steamIDUser, pCallBack->m_iMessageID, tmp, 4096, &entry);

	if (entry != k_EChatEntryTypeChatMsg && entry != k_EChatEntryTypeEmote) {
		return;
	}
	else
	{
		MessageReceivedCallbackAutoReply(pCallBack, tmp);
	}
}

#else 

void SteamAPIManager::SteamManualCallback()
{
	HSteamPipe hSteamPipe = SteamAPI_GetHSteamPipe();
	SteamAPI_ManualDispatch_RunFrame(hSteamPipe);
	CallbackMsg_t callback;

	while (SteamAPI_ManualDispatch_GetNextCallback(hSteamPipe, &callback))
	{
		// Check for dispatching API call results
		if (callback.m_iCallback == SteamAPICallCompleted_t::k_iCallback)
		{
			SteamAPICallCompleted_t* pCallCompleted = (SteamAPICallCompleted_t*)callback.m_pubParam;
			void* pTmpCallResult = malloc(callback.m_cubParam);
			bool bFailed;

			if (SteamAPI_ManualDispatch_GetAPICallResult(hSteamPipe, pCallCompleted->m_hAsyncCall, pTmpCallResult, pCallCompleted->m_cubParam, pCallCompleted->m_iCallback, &bFailed))
			{
				// Dispatch the call result to the registered handler(s) for the
				// call identified by pCallCompleted->m_hAsyncCall
				pCallCompleted->m_hAsyncCall;

				// Handlers

				printf_s("SteamAPI_ManualDispatch_GetAPICallResult\n");
			}
			else
			{
				printf_s("Error getting call result\n");
			}
			free(pTmpCallResult);
		}
		else if(callback.m_iCallback == 331)// Cas de l'overlay
		{
			OnGameOverlayActivated((GameOverlayActivated_t*)callback.m_pubParam);
			printf_s("Appel du callback de l'overlay \n");
		}
		if (callback.m_iCallback == 343)//Cas du Message Ami
		{
			OnMessageReceived((GameConnectedFriendChatMsg_t*)callback.m_pubParam);
			printf_s("Appel du callback du message steam \n");
		}
		else
		{
			// Look at callback.m_iCallback to see what kind of callback it is,
			// and dispatch to appropriate handler(s)
			
			printf_s("Appel du callback %d \n", callback.m_iCallback);

			printf_s("Callback cible %d \n", SteamAPICallCompleted_t::k_iCallback);

			//304
			//331 Activation Overlay Steam
			//343 Message ami
			//348 Ami commence à écrire un message
			//501
			//502
			//703 SteamAPICallCompleted_t::k_iCallback
			//1040015 Focalisation ou Défocalisation sur la fenêtre de jeu.
		}
		SteamAPI_ManualDispatch_FreeLastCallback(hSteamPipe);
	}
}

void SteamAPIManager::OnGameOverlayActivated(GameOverlayActivated_t* pCallback, bool bIOFailure)
{
	if (bIOFailure)
	{
		printf("Échec lors de l'appel du callback OnGameOverlayActivated.\n");
		return;
	}

	if (pCallback->m_bActive)
		printf_s("\nL'overlay Steam est à présent active !\n");
	else
		printf_s("\nL'overlay Steam est à présent inactive !\n");
}

//Will intercept the message and reply if valid
void SteamAPIManager::OnMessageReceived(GameConnectedFriendChatMsg_t* pCallBack, bool bIOFailure)
{
	if (bIOFailure)
	{
		printf("Échec lors de l'appel du callback OnMessageReceived.\n");
		return;
	}
	EChatEntryType entry = k_EChatEntryTypeInvalid;
	char* tmp = new char[4096];
	SteamFriends()->GetFriendMessage(pCallBack->m_steamIDUser, pCallBack->m_iMessageID, tmp, 4096, &entry);

	if (entry != k_EChatEntryTypeChatMsg && entry != k_EChatEntryTypeEmote) {
		return;
	}
	else
	{
		MessageReceivedCallbackAutoReply(pCallBack, tmp);
	}
}


#endif

//Auto reply message msg to sender
void SteamAPIManager::MessageReceivedCallbackAutoReply(GameConnectedFriendChatMsg_t* pMessageCallBack, const char* msg)
{

	SteamFriends()->ReplyToFriendMessage(pMessageCallBack->m_steamIDUser, msg);
	std::cout << std::endl;
	std::cout << "MESSAGE RECEIVED" << std::endl << std::endl;
	std::cout << "FROM (STEAMID) : " << pMessageCallBack->m_steamIDUser.ConvertToUint64() << std::endl;
	std::cout << "FROM (NAME) : " << SteamFriends()->GetFriendPersonaName(pMessageCallBack->m_steamIDUser) << std::endl;
	std::cout << "MESSAGE ID : " << pMessageCallBack->m_iMessageID << std::endl;
	std::cout << "AUTO REPLIED TO MESSAGE " << pMessageCallBack->m_iMessageID << ": " << msg << std::endl;
	std::cout << std::endl;
}
