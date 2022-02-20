#pragma once
#ifndef SDLMANAGER_H
#define SDLMANAGER_H

#include "../sdl/include/sdl.h"
#include "../sdl/include/sdl_image.h"
#include <iostream>
#include <string>
#include <stdio.h>


class SDLManager
{
private:
	const int SCREEN_WIDTH = 1600;
	const int SCREEN_HEIGHT = 900;
	SDL_Window* window;
	SDL_Surface* image;
	SDL_Renderer* renderer;
	SDL_Texture* texture;

public:
	SDLManager();

	bool SDLInit();
	void SDLQuit();
	void SDLUpdate();
};

#endif