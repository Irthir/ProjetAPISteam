#pragma once
#ifndef STEAMAPIMANAGER_H
#define STEAMAPIMANAGER_H

#include "../steam/steam_api.h"
#include "../steam/isteamuserstats.h"
//sdkencryptedappticket64.lib;
#include <iostream>
#include <stdio.h>
class SteamAPIManager
{
private:
    STEAM_CALLBACK(SteamAPIManager, OnGameOverlayActivated, GameOverlayActivated_t);
    STEAM_CALLBACK(SteamAPIManager, OnMessageReceived, GameConnectedFriendChatMsg_t); //Will trigger when a friend send a message
    void MessageReceivedCallbackAutoReply(GameConnectedFriendChatMsg_t* pMessageCallBack, const char* msg);//Auto reply message msg to sender
public:
    SteamAPIManager();
    ~SteamAPIManager();

    bool SteamAPIInit();
    void SteamAPIQuit();
    void SteamUpdate();
    void ActivateOverlay();
};

#endif#pragma once
