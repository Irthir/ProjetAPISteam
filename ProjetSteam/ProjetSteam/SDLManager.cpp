#include "SDLManager.h"

SDLManager::SDLManager()
{
	this->window = NULL;
	this->image = NULL;
	this->renderer = NULL;
	this->texture = NULL;
}

bool SDLManager::SDLInit()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << ("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	else
	{
		//Init IMG;
		int imgFlags = (IMG_INIT_JPG | IMG_INIT_PNG);
		IMG_Init(imgFlags);
		if (!(IMG_Init(imgFlags) && imgFlags))
		{
			std::cout << "SDL_Image could not be initialized. SDL_Image Error : " << IMG_GetError() << "\n";
			return false;
		}

		//Create window
		window = SDL_CreateWindow("Half Life 3", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			std::cout << "Window could not be created! SDL_Error: \n" << SDL_GetError();
			return false;
		}
		else
		{
			//Get window surface
			image = SDL_GetWindowSurface(window);

			//Load IMG
			image = IMG_Load("res/png/halflife3.png");
			if (!image)
			{
				std::cout << "error creating surface: \n" << SDL_GetError() << "\n";
				return false;
			}
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			if (renderer == NULL)
			{
				std::cout << "We were not able to create the renderer. SDL Error " << SDL_GetError() << "\n";
				return false;
			}
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

			texture = SDL_CreateTextureFromSurface(renderer, image);
			if (texture == NULL)
			{
				std::cout << "Failed to load the image.\n";
				return false;
			}

			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
			return true;
		}
	}
}

void SDLManager::SDLQuit()
{
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	//Quit IMG
	IMG_Quit();

	//Destroy window
	SDL_FreeSurface(image);
	SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();
}

bool SDLManager::SDLUpdate()
{
	bool bRunning = true;
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			bRunning = false;
			break;
		case SDL_MOUSEBUTTONDOWN:
			bRunning = false;
			break;
		default:
			break;
		}
	}
	return bRunning;
}