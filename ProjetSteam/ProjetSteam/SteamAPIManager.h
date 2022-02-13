#pragma once
#ifndef STEAMAPIMANAGER_H
#define STEAMAPIMANAGER_H
#include <steam_api.h>
#include <direct.h>

class SteamAPIManager
{
private:
	//STEAM_CALLBACK(SteamAPIManager, OnGameOverlayActivated, GameOverlayActivated_t);

public:
	SteamAPIManager();

	bool SteamAPIInit();
	void SteamAPIQuit();
	void SteamUpdate();
	void ActivateOverlay();
};

#endif