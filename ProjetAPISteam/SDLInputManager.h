#pragma once
#ifndef SDLINPUTMANAGER_H
#define SDLINPUTMANAGER_H

#include "../sdl/include/sdl.h"
#include "GameManager.h"
#include <iostream>
#include <string>
#include <stdio.h>


class SDLInputManager
{
private:
	GameManager* pGameManager;
public:
	SDLInputManager(GameManager* pGameManager);
	
	void SDLKeyInputHandler(const char* cInput);
	bool SDLInputUpdate();
};

#endif