#include <SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include <stdio.h>
#include <string.h>
#define _MAIN 5
#include"functions.h"
#include<stdlib.h>


SDL_Window* main_window = NULL;
SDL_Renderer* main_renderer = NULL;
//Globally used font
TTF_Font *gFont = NULL;

Mix_Music *gMusic = NULL;
Mix_Chunk *shot_fired = NULL;
Mix_Chunk *pow_bomb = NULL;
Mix_Chunk *pow_life = NULL;
Mix_Chunk *pow_shovel = NULL;
Mix_Chunk *pow_shield = NULL;
Mix_Chunk *pow_star = NULL;
Mix_Chunk *die = NULL;
Mix_Chunk *pow_clock = NULL;
Mix_Chunk *gameover = NULL;

//Ucitavanje (window, renderer, sdl_video)
int initializeSDL(int SCREEN_WIDTH, int SCREEN_HEIGHT) {
	int success = 1;
	int imgFlags;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = 0;
	}
	else
	{
		main_window = SDL_CreateWindow("Battle Tanks", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT+20, SDL_WINDOW_SHOWN);
		if (!main_window)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = 0;
		}
		else
		{
			main_renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED);
			if (!main_renderer)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = 0;
			}
			else
			{
				SDL_SetRenderDrawColor(main_renderer, 255, 215, 0, 255);


				imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = 0;
				}
				
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
				{
					printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
					success = 0;
				}

				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = 0;
				}

				//Load music
				gMusic = Mix_LoadMUS("gfx/sound/bob_marley.mp3");
				if (gMusic == NULL)
				{
					printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
					success = 0;
				}

				//Load sound effects
				shot_fired = Mix_LoadWAV("gfx/sound/gun_shot.wav");
				if (shot_fired == NULL)
				{
					printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
					success = 0;
				}

				pow_bomb = Mix_LoadWAV("gfx/sound/bomb.wav");
				if (shot_fired == NULL)
				{
					printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
					success = 0;
				}
				pow_life = Mix_LoadWAV("gfx/sound/life.wav");
				if (shot_fired == NULL)
				{
					printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
					success = 0;
				}
				pow_shovel = Mix_LoadWAV("gfx/sound/shovel.wav");
				if (shot_fired == NULL)
				{
					printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
					success = 0;
				}
				pow_shield = Mix_LoadWAV("gfx/sound/shield.wav");
				if (shot_fired == NULL)
				{
					printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
					success = 0;
				}
				pow_star = Mix_LoadWAV("gfx/sound/star.wav");
				if (shot_fired == NULL)
				{
					printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
					success = 0;
				}
				pow_clock = Mix_LoadWAV("gfx/sound/clock.wav");
				if (shot_fired == NULL)
				{
					printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
					success = 0;
				}
				die = Mix_LoadWAV("gfx/sound/die.wav");
				if (shot_fired == NULL)
				{
					printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
					success = 0;
				}
				gameover = Mix_LoadWAV("gfx/sound/gameover.mp3");
				if (shot_fired == NULL)
				{
					printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
					success = 0;
				}


			}
		}

	}
	return success;
}

//Dealociranje sve zauzete memorije (windows, textures, renderers, surfaces) i SDL_quit
void close_SDL() {
	//+close font
	SDL_DestroyRenderer(main_renderer);
	SDL_DestroyWindow(main_window);
	Mix_FreeChunk(shot_fired);
	Mix_FreeMusic(gMusic);
	gMusic = NULL;
	main_window = NULL;
	main_renderer = NULL;
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

//Ucitavanje tekstura (svih slicica tenkica, cigli, vode, . . .
int loadTextures( SDL_Texture* player[TOTAL], SDL_Texture* bot1[TOTAL], SDL_Texture* bot2[TOTAL], SDL_Texture* elements[11]) {
	SDL_Surface* surface = NULL;
	int success = 1;

	surface = IMG_Load("gfx/brick.png");
	elements[BRICK] = SDL_CreateTextureFromSurface(main_renderer, surface);
	IFNOT(elements[BRICK]);
	SDL_FreeSurface(surface);
	surface = NULL;


	surface = IMG_Load("gfx/bullet.png");
	elements[BULLET] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(elements[BULLET]);

	surface = IMG_Load("gfx/eagle.png");
	elements[EAGLE] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(elements[EAGLE]);

	surface = IMG_Load("gfx/1.png");
	SDL_SetWindowIcon(main_window, surface);
	player[UP] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(player[UP]);

	surface = IMG_Load("gfx/2.png");
	player[DOWN] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(player[DOWN]);

	surface = IMG_Load("gfx/3.png");
	player[LEFT] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(player[LEFT]);

	surface = IMG_Load("gfx/4.png");
	player[RIGHT] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(player[RIGHT]);

	surface = IMG_Load("gfx/background.png");
	elements[BLACK] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(elements[BLACK]);

	surface = IMG_Load("gfx/steel.png");
	elements[STEEL] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(elements[STEEL]);

	surface = IMG_Load("gfx/water.png");
	elements[WATER] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(elements[WATER]);

	surface = IMG_Load("gfx/life.png");
	elements[LIFE] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(elements[LIFE]);

	surface = IMG_Load("gfx/bomb.png");
	elements[BOMB] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(elements[BOMB]);

	surface = IMG_Load("gfx/star.png");
	elements[STAR] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(elements[STAR]);

	surface = IMG_Load("gfx/clock.png");
	elements[CLOCK] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(elements[CLOCK]);

	surface = IMG_Load("gfx/shovel.png");
	elements[SHOVEL] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(elements[SHOVEL]);

	surface = IMG_Load("gfx/shield.png");
	elements[SHIELD] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(elements[SHIELD]);

	surface = IMG_Load("gfx/bot1/2.png");
	bot1[DOWN] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(bot1[DOWN]);

	surface = IMG_Load("gfx/bot1/1.png");
	bot1[UP] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(bot1[UP]);

	surface = IMG_Load("gfx/bot1/3.png");
	bot1[LEFT] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(bot1[LEFT]);

	surface = IMG_Load("gfx/bot1/4.png");
	bot1[RIGHT] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(bot1[RIGHT]);

	surface = IMG_Load("gfx/bot2/2.png");
	bot2[DOWN] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(bot2[DOWN]);

	surface = IMG_Load("gfx/bot2/1.png");
	bot2[UP] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(bot2[UP]);

	surface = IMG_Load("gfx/bot2/3.png");
	bot2[LEFT] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(bot2[LEFT]);

	surface = IMG_Load("gfx/bot2/4.png");
	bot2[RIGHT] = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	IFNOT(bot2[RIGHT]);

	//+open fonts, create surfaces or textures


	return success;
}

//Dealocira teksture koristene u f-ji startGame
void freeTextures(SDL_Texture* player[TOTAL], SDL_Texture* bot1[TOTAL], SDL_Texture* bot2[TOTAL], SDL_Texture* elements[TOTAL_ELEMENTS]) {
	int i;

	for (i = 0; i < TOTAL-1; i++) {
		SDL_DestroyTexture(player[i]);
		player[i] = NULL;
	}

	for (i = 0; i < TOTAL-1; i++) {
		SDL_DestroyTexture(bot1[i]);
		bot1[i] = NULL;
	}
	
	for (i = 0; i < TOTAL-1; i++) {
		SDL_DestroyTexture(bot2[i]);
		bot2[i] = NULL;
	}

	for (i = 0; i < TOTAL_ELEMENTS; i++) {
		SDL_DestroyTexture(elements[i]);
		elements[i] = NULL;
	}
}




/*


bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
//Get rid of preexisting texture
free();

//Render text surface
SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
if( textSurface == NULL )
{
printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
}
else
{
//Create texture from surface pixels
mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
if( mTexture == NULL )
{
printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
}
else
{
//Get image dimensions
mWidth = textSurface->w;
mHeight = textSurface->h;
}

//Get rid of old surface
SDL_FreeSurface( textSurface );
}

//Return success
return mTexture != NULL;
}


*/



/*

bool loadMedia()
{
//Loading success flag
bool success = true;

//Open the font
gFont = TTF_OpenFont( "16_true_type_fonts/lazy.ttf", 28 );
if( gFont == NULL )
{
printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
success = false;
}
else
{

//Render text
SDL_Color textColor = { 0, 0, 0 };
if( !gTextTexture.loadFromRenderedText( "The quick brown fox jumps over the lazy dog", textColor ) )
{
printf( "Failed to render text texture!\n" );
success = false;
}
}

return success;
}


*/