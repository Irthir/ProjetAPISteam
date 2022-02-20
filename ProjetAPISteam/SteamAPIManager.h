#pragma once
#ifndef STEAMAPIMANAGER_H
#define STEAMAPIMANAGER_H

#include "../steam/steam_api.h"
#include "../steam/isteamuserstats.h"
#include <iostream>
#include <stdio.h>

//Décommenter pour passer aux callbacks manuels
#define STEAMMANUALCALLBACK 

class SteamAPIManager
{
private:
#ifndef STEAMMANUALCALLBACK

    STEAM_CALLBACK(SteamAPIManager, OnGameOverlayActivated, GameOverlayActivated_t);
    STEAM_CALLBACK(SteamAPIManager, OnMessageReceived, GameConnectedFriendChatMsg_t); //Réagit quand un ami envoie un message

#else
    void SteamManualCallback();

    void OnGameOverlayActivated(GameOverlayActivated_t* pCallback, bool bIOFailure=false);
    CCallResult< SteamAPIManager, GameOverlayActivated_t > m_GameOverlayActivated; //UTILISATION DES CALLRESULTS

    void OnMessageReceived(GameConnectedFriendChatMsg_t* pCallback, bool bIOFailure = false);
    CCallResult< SteamAPIManager, GameConnectedFriendChatMsg_t > m_GameConnectedFriendChatMsg; //UTILISATION DES CALLRESULTS
#endif
    
void MessageReceivedCallbackAutoReply(GameConnectedFriendChatMsg_t* pMessageCallBack, const char* msg); //Répond automatiquement aux messages reçus


public:
    SteamAPIManager();
    ~SteamAPIManager();

    bool SteamAPIInit();
    void SteamAPIQuit();
    void SteamUpdate();

    void ActivateOverlay();
};

#endif