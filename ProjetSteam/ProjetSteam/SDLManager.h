#pragma once
#ifndef SDLMANAGER_H
#define SDLMANAGER_H
#include <stdio.h>
#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

class SDLManager
{
private :
	const int SCREEN_WIDTH = 1600;
	const int SCREEN_HEIGHT = 900;
	SDL_Window* window;
	SDL_Surface *image;
	SDL_Renderer *renderer;
	SDL_Texture *texture;

public :
	SDLManager();
	~SDLManager();

	bool SDLInit();
	void SDLQuit();
	bool SDLUpdate();
};

#endif