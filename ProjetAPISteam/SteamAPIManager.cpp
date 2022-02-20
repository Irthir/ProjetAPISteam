#include "SteamAPIManager.h"

SteamAPIManager::SteamAPIManager()
//Constructeur
{
	bHote=false;
}

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

	//Recherche de lobby
	SteamMatchmaking()->AddRequestLobbyListResultCountFilter(1000000000);
	SteamMatchmaking()->AddRequestLobbyListStringFilter("NOM", "PATATE", ELobbyComparison::k_ELobbyComparisonEqual);

	SteamAPICall_t hSteamAPICall = SteamMatchmaking()->RequestLobbyList();


#ifndef STEAMMANUALCALLBACK
	m_LobbyMatchListCallResult.Set(hSteamAPICall, this, &SteamAPIManager::OnGetLobbyMatchList);
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
//BUT : Gérer l'appel des évènements
{
#ifndef STEAMMANUALCALLBACK
	SteamAPI_RunCallbacks();
#else
	SteamManualCallback();
#endif
}

void SteamAPIManager::ActivateOverlay()
//BUT : Activer l'overlay steam.
//NOTE : Active l'évènement d'ouverture qui est reçu par steam, mais l'overlay ne s'affiche pas.
{
	printf_s("Activation de l'overlay Steam\n");
	SteamFriends()->ActivateGameOverlay("Friends");
}

#ifndef STEAMMANUALCALLBACK

void SteamAPIManager::OnGameOverlayActivated(GameOverlayActivated_t* pCallback)
//BUT : Réagir quand l'overlay s'active ou se désactive.
{

	if (pCallback->m_bActive)
		printf_s("\nL'overlay Steam est à présent active !\n");
	else
		printf_s("\nL'overlay Steam est à présent inactive !\n");
}


void SteamAPIManager::OnMessageReceived(GameConnectedFriendChatMsg_t* pCallBack)
//BUT : Récupère les messages reçus et répond.
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

//SUITE PAS ENCORE INTEGRÉE POUR LES CALLBACKS MANUELS !!!
void SteamAPIManager::OnGetLobbyMatchList(LobbyMatchList_t* pCallback, bool bIOFailure)
//BUT : Récupérer la liste des lobbys et se connecter ou en créer un le cas échéant.
{
	if (bIOFailure)
	{
		std::cout << "Échec dans la recherche de lobby." << std::endl;
		return;
	}

	std::cout << "Nombre de lobbys existant : " << pCallback->m_nLobbiesMatching << std::endl;

	bool bLobby = false;

	for (int i = 0; i < pCallback->m_nLobbiesMatching; i++)
	{
		CSteamID LobbyID = SteamMatchmaking()->GetLobbyByIndex(i);
		if (LobbyID.IsLobby())
		{
			steamIDLobby = LobbyID;
			bLobby = true;
			break;
		}
	}

	if (bLobby)
	{
		printf_s("Lobby trouvé %lld.\n", steamIDLobby);

		const char* cNomLobby = SteamMatchmaking()->GetLobbyData(steamIDLobby, "NOM");

		printf_s("Nom du lobby %s.\n", cNomLobby);

		//Rejoindre le Lobby trouvé
		SteamAPICall_t hSteamAPICall = SteamMatchmaking()->JoinLobby(steamIDLobby);

		m_LobbyEnterCallResult.Set(hSteamAPICall, this, &SteamAPIManager::OnLobbyEntered);

		m_LobbyChatUpdate.Set(hSteamAPICall, this, &SteamAPIManager::OnLobbyChatUpdate);
	}
	else
	{
		printf_s("Lobby non trouvé %lld.\n", steamIDLobby);
		//Création d'un Lobby
		LobbyCreation();
	}
}

void SteamAPIManager::OnLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure)
//BUT : Évènement de création d'un lobby.
{
	if (bIOFailure || !pCallback->m_eResult)
	{
		printf_s(" Création de lobby échouée! \n");
		return;
	}

	printf_s("Lobby créé %lld.\n", pCallback->m_ulSteamIDLobby);


	steamIDLobby = pCallback->m_ulSteamIDLobby;

	if (SteamMatchmaking()->SetLobbyData(steamIDLobby, "NOM", "PATATE"))
	{
		printf_s("Mise en place des données du lobby.\n");
	}
	else
	{
		printf_s("Erreur dans la mise en place des données du lobby.\n");
	}

	const char* cNomLobby = SteamMatchmaking()->GetLobbyData(steamIDLobby, "NOM");
	printf_s("Nom du lobby %s.\n", cNomLobby);

	if (SteamMatchmaking()->SetLobbyJoinable(steamIDLobby, true))
	{
		printf_s("Lobby ouvert %s.\n", cNomLobby);
	}
	else
	{
		printf_s("Erreur dans l'ouverture du lobby.\n");
	}

	if (SteamMatchmaking()->SetLobbyMemberLimit(steamIDLobby, 10))
	{
		printf_s("Lobby limité à %d joueurs.\n", 10);
	}
	else
	{
		printf_s("Erreur dans la limitation du lobby.\n");
	}
}

void SteamAPIManager::OnLobbyEntered(LobbyEnter_t* pCallback, bool bIOFailure)
//BUT : Évènement d'entrée dans un lobby.
{
	if (bIOFailure)
	{
		std::cout << "Échec dans l'accession à un lobby." << std::endl;
		return;
	}

	CSteamID nIdLobby = pCallback->m_ulSteamIDLobby;

	printf_s("Lobby rejoint : %lld.\n", nIdLobby);

	int nbJoueur = SteamMatchmaking()->GetNumLobbyMembers(pCallback->m_ulSteamIDLobby);

	printf_s("Nombre de joueurs présents dans le lobby %d.\n", nbJoueur);

	for (int i = 0; i < nbJoueur; i++)
	{
		CSteamID UserID = SteamMatchmaking()->GetLobbyMemberByIndex(pCallback->m_ulSteamIDLobby, i);
		printf_s("Joueur %d :\n", i+1);

		const char* userName = SteamFriends()->GetFriendPersonaName(UserID);
		std::cout << "Nom d'utilisateur : " << userName << std::endl;

		const char* oldUserName = SteamFriends()->GetFriendPersonaNameHistory(UserID, 0);
		std::cout << "Ancien nom utilisateur : " << oldUserName << std::endl;
	}

	m_LobbyEnterCallResult.Set(pCallback->m_ulSteamIDLobby, this, &SteamAPIManager::OnLobbyEntered);
	m_LobbyChatMsg.Set(pCallback->m_ulSteamIDLobby, this, &SteamAPIManager::OnLobbyChatMessage);
}

void SteamAPIManager::OnLobbyChatUpdate(LobbyChatUpdate_t* pCallback, bool bIOFailure)
//BUT : Évènement d'entrée ou de sortie des joueurs dans le lobby.
{
	if (bIOFailure)
	{
		std::cout << "Échec dans la mise à jour du lobby." << std::endl;
		return;
	}
	

	const char* sMessage;
	switch (pCallback->m_rgfChatMemberStateChange)
	{
	case (k_EChatMemberStateChangeEntered):
		sMessage = "est entré dans le lobby.";
		break;
	case (k_EChatMemberStateChangeLeft):
		sMessage = "a quitté le lobby.";
		break;
	case (k_EChatMemberStateChangeDisconnected):
		sMessage = "s'est déconnecté.";
		break;
	case (k_EChatMemberStateChangeKicked):
		sMessage = "a été exclu.";
		break;
	case (k_EChatMemberStateChangeBanned):
		sMessage = "a été banni.";
		break;
	default:
		sMessage = ".";
		break;
	}


	const char* sNom = SteamFriends()->GetFriendPersonaName(pCallback->m_ulSteamIDMakingChange);

	printf_s("%s %s\n",sNom, sMessage);
}

void SteamAPIManager::OnLobbyChatMessage(LobbyChatMsg_t* pCallback, bool bIOFailure)
//BUT : Évènement de réception d'un message du lobby.
{
	if (bIOFailure)
	{
		std::cout << "Échec dans la mise à jour du lobby." << std::endl;
		return;
	}
	
	char* buffer = new char[256];

	SteamMatchmaking()->GetLobbyChatEntry(steamIDLobby, pCallback->m_iChatID, (CSteamID*)pCallback->m_ulSteamIDUser, buffer, 8, nullptr);

	printf_s("Message reçu : \"%s\"", buffer);

	delete[] buffer;
}


#else 

void SteamAPIManager::SteamManualCallback()
//BUT : Gérer les callbacks manuels
{
	HSteamPipe hSteamPipe = SteamAPI_GetHSteamPipe();
	SteamAPI_ManualDispatch_RunFrame(hSteamPipe);
	CallbackMsg_t callback;

	while (SteamAPI_ManualDispatch_GetNextCallback(hSteamPipe, &callback))
	{
		// Check for dispatching API call results
		if (callback.m_iCallback == SteamAPICallCompleted_t::k_iCallback) //Ne réagit jamais dans l'état actuel car le résultat ne vaut jamais 703
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
		else if(callback.m_iCallback == 331)//Cas de l'overlay
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
//BUT : Évènement d'activation de l'overlay, identique à celui plus haut.
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


void SteamAPIManager::OnMessageReceived(GameConnectedFriendChatMsg_t* pCallBack, bool bIOFailure)
//BUT : Évènement de réception d'un message Steam.
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


void SteamAPIManager::MessageReceivedCallbackAutoReply(GameConnectedFriendChatMsg_t* pMessageCallBack, const char* msg)
//BUT : Répondre à un message Steam en répondant la même chose que ce qui est reçu.
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


void SteamAPIManager::LobbyCreation()
//BUT : Créer un lobby.
{
	SteamAPICall_t hSteamAPICall = SteamMatchmaking()->CreateLobby(k_ELobbyTypePublic, 2);

#ifndef STEAMMANUALCALLBACK
	m_LobbyCreatedCallResult.Set(hSteamAPICall, this, &SteamAPIManager::OnLobbyCreated);

	m_LobbyEnterCallResult.Set(hSteamAPICall, this, &SteamAPIManager::OnLobbyEntered);

	m_LobbyChatUpdate.Set(hSteamAPICall, this, &SteamAPIManager::OnLobbyChatUpdate);

	m_LobbyChatMsg.Set(hSteamAPICall, this, &SteamAPIManager::OnLobbyChatMessage);
#endif
}

void SteamAPIManager::SendMessage()
//BUT : Envoyer un message.
{
	const char* buffer = "Patate.";

	if (SteamMatchmaking()->SendLobbyChatMsg(steamIDLobby, "Patate.", 8))
	{
		printf_s("\"%s\" envoyé !\n", buffer);
	}
	else
	{
		printf_s("Échec lors de l'envoie de \"%s\" !\n", buffer);
	}
}