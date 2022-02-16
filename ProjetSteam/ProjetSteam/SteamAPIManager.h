#pragma once
#ifndef STEAMAPIMANAGER_H
#define STEAMAPIMANAGER_H

#include "../public/steam/steam_api.h"
#include "../public/steam/isteamfriends.h"
#include <direct.h>

class SteamAPIManager
{
private:
	STEAM_CALLBACK( SteamAPIManager, OnFriendStatusChanged, PersonaStateChange_t);
public:
	SteamAPIManager();
	~SteamAPIManager();

	bool SteamAPIInit();
	void SteamAPIQuit();
	void SteamUpdate();
	void ActivateOverlay();
};

#endif