#pragma once
#ifndef STEAMAPIMANAGER_H
#define STEAMAPIMANAGER_H
#include <steam_api.h>

class SteamAPIManager
{
private:

public:
	SteamAPIManager();

	bool SteamAPIInit();
	void SteamAPIQuit();
};

#endif