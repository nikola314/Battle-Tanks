#include <SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include <stdio.h>
#include <string.h>
#include "functions.h"
#include<stdlib.h>
#include<time.h>


int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 640;
int DIFFICULTY = 1;
int tank_index;
int MAX_LEVELS = 10;
int ***game_maps = NULL;
int tank_powerups[5];
int change_screen = 0;


int main(int argc, char* argv[]) {
	

	int user_choice;
	int done = 0;
	SDL_Texture* user_tank[TOTAL];
	SDL_Texture* bot_tank1[TOTAL];
	SDL_Texture* bot_tank2[TOTAL];
	SDL_Texture* map_elements[TOTAL_ELEMENTS];


	initializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);

	srand(time(NULL));
	

	//Ako nisu dobro ucitane teksture( gasi SDL i ispisuje poruku )
	if (!loadTextures(user_tank, bot_tank1, bot_tank2, map_elements)) {
		printf("Failed to load textures!\n");
		close_SDL(main_window, main_renderer);
		exit(1);
	}
	
	Mix_PlayMusic(gMusic, -1);

	while (!done) {
		user_choice = menu_gui();

		switch (user_choice) {
		case OPTION_PLAY:	done=startGame(user_tank, bot_tank1, bot_tank2, map_elements); break;
		case OPTION_SETTINGS: settingsScreen(user_tank, bot_tank1, bot_tank2, map_elements); break;
		case OPTION_HIGH_SCORES: highScoresScreen();  break;
		case OPTION_INFO:infoScreen(); break;
			/* Ostale opcije */
			/* Strukture su definisane u .h fajlu*/

		case OPTION_EXIT: close_SDL(); return 0;
		}
	}
	return 0;
}