#include <SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include <stdio.h>
#include <string.h>
#include "functions.h"
#include<stdlib.h>


//funkcija koja crta meni i vraca izabranu opciju - poziva funkciju paintMenu zbog crtanja
int menu_gui() {
	int option = 0;
	int out = 0;
	SDL_Event ev;
	const Uint8* key_state;

	while (1) {
		paintMenu(option);
		while (SDL_PollEvent(&ev) != 0) {
			if (ev.type == SDL_QUIT) {
				option = OPTION_EXIT;
				out = 1;
			}
			else if (ev.type == SDL_KEYDOWN) {
				switch (ev.key.keysym.sym) {
				case SDLK_UP:
					if (!option) option = OPTION_TOTAL - 1;
					else option--; break;

				case SDLK_DOWN:
					if (option == OPTION_TOTAL - 1) option = 0;
					else option++; break;

				case SDLK_SPACE: out = 1; break;

				case SDLK_m:
					if (Mix_PlayingMusic() == 0)
					//Play the music
					Mix_PlayMusic(gMusic, -1);
					//If music is being played
							 else
							 {
								 //If the music is paused
								 if (Mix_PausedMusic() == 1)
									 Mix_ResumeMusic();
								 //If the music is playing
								 else
									 Mix_PauseMusic();
							 }
							 break;
				}
			}
			

		}
		key_state = SDL_GetKeyboardState(NULL);

		if (key_state[SDL_SCANCODE_LSHIFT] && key_state[SDL_SCANCODE_R] && key_state[SDL_SCANCODE_S])
			restore();

		if (out) break;
	}
	return option;
}

//funkcija koja crta settings meni i vrsi izmene nad dimenzijama i tezinom - poziva funkciju paintSettings zbog crtanja
void settingsScreen(SDL_Texture* user_tank[TOTAL], SDL_Texture* bot_tank1[TOTAL], SDL_Texture* bot_tank2[TOTAL], SDL_Texture* map_elements[TOTAL_ELEMENTS]) {
	int option = 0;
	int out = 0;
	int exit_game = 0;
	SDL_Event ev;
	const Uint8* key_state;

	while (1) {
		paintSettings(option);
		while (SDL_PollEvent(&ev) != 0) {
			if (ev.type == SDL_QUIT) {
				option = OPTION_EXIT;
				out = 1;
				//ne out=1 vec close SDL. . . itd.
			}
			else if (ev.type == SDL_KEYDOWN) {
				switch (ev.key.keysym.sym) {
				case SDLK_UP:
					if (!option) option = OPTION_TOTAL-3 ;
					else option--; break;

				case SDLK_DOWN:
					if (option == OPTION_TOTAL -3) option = 0;
					else option++; break;

				case SDLK_SPACE:

					switch (option) {
					case 0: switch (DIFFICULTY) {
					case 1: DIFFICULTY = 2; break;
					case 2: DIFFICULTY = 3; break;
					case 3: DIFFICULTY = 4; break;
					case 4: DIFFICULTY = 1; break;
					}
							break;
					case 1:
						if (SCREEN_WIDTH > LITTLE_WIDTH) {
							SCREEN_WIDTH = LITTLE_WIDTH;
							SCREEN_HEIGHT = LITTLE_HEIGHT;
						
						}
						else {
							SCREEN_WIDTH = BIG_WIDTH;
							SCREEN_HEIGHT = BIG_HEIGHT;
						
						}
						
						SDL_SetWindowSize(main_window, SCREEN_WIDTH, SCREEN_HEIGHT + 20);
						SDL_DestroyRenderer(main_renderer);
						main_renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED);
						SDL_SetRenderDrawColor(main_renderer, PRIM_COLOR_R, PRIM_COLOR_G, PRIM_COLOR_B, 255);
						if (!loadTextures(user_tank, bot_tank1, bot_tank2, map_elements)) {
							printf("Failed to load textures!\n");
							close_SDL(main_window, main_renderer);
							exit(1);
						}
						break;
						/* ostale opcije */
					case 2:createMaps(map_elements); break;
					}
					break;

				case SDLK_ESCAPE: out = 1; break;
				case SDLK_m:
					if (Mix_PlayingMusic() == 0)
						//Play the music
						Mix_PlayMusic(gMusic, -1);
					//If music is being played
					else
					{
						//If the music is paused
						if (Mix_PausedMusic() == 1)
							Mix_ResumeMusic();
						//If the music is playing
						else
							Mix_PauseMusic();
					}
					break;
				}
			}
		}
		key_state = SDL_GetKeyboardState(NULL);

		if (key_state[SDL_SCANCODE_LSHIFT] && key_state[SDL_SCANCODE_R] && key_state[SDL_SCANCODE_S])
			restore();

		if (out) break;
	}
	SDL_SetRenderDrawColor(main_renderer, PRIM_COLOR_R, PRIM_COLOR_G, PRIM_COLOR_B, 255);
}


//funkcija koja crta high scores meni  - poziva funkciju paintHighScores zbog crtanja
void highScoresScreen() {
	int option = 0;
	int out = 0;
	int exit_game = 0;
	SDL_Event ev;

	while (1) {
		paintHighScores();
		while (SDL_PollEvent(&ev) != 0) {
			if (ev.type == SDL_QUIT) {
				option = OPTION_EXIT;
				out = 1;
				//ne out=1 vec close SDL. . . itd.
			}
			else if (ev.type == SDL_KEYDOWN) {
				if (ev.key.keysym.sym == SDLK_ESCAPE) {
					out = 1;
				}

				if (ev.key.keysym.sym == SDLK_m) {
					if (Mix_PlayingMusic() == 0)
						//Play the music
						Mix_PlayMusic(gMusic, -1);
					//If music is being played
					else
					{
						//If the music is paused
						if (Mix_PausedMusic() == 1)
							Mix_ResumeMusic();
						//If the music is playing
						else
							Mix_PauseMusic();
					}
				}
			}
		}
		if (out) break;
	}
	SDL_SetRenderDrawColor(main_renderer, PRIM_COLOR_R, PRIM_COLOR_G, PRIM_COLOR_B, 255);
}

void infoScreen() {
	int option = 0;
	int out = 0;
	int exit_game = 0;
	SDL_Event ev;

	while (1) {
		paintInfo();
		while (SDL_PollEvent(&ev) != 0) {
			if (ev.type == SDL_QUIT) {
				option = OPTION_EXIT;
				out = 1;
				//ne out=1 vec close SDL. . . itd.
			}
			else if (ev.type == SDL_KEYDOWN) {
				if (ev.key.keysym.sym == SDLK_ESCAPE) {
					out = 1;
				}
			}
		}
		if (out) break;
	}
	SDL_SetRenderDrawColor(main_renderer, PRIM_COLOR_R, PRIM_COLOR_G, PRIM_COLOR_B, 255);
}

void createMaps(SDL_Texture* map_elements[TOTAL_ELEMENTS]) {
	int current_map = 1;
	int i;
	int out = 0;
	SDL_Event ev;
	int x = 0, y = 0;

	if (game_maps) dealok(game_maps);
	game_maps = NULL;
	game_maps = getMaps2(game_maps);
	change_screen = SCREEN_HEIGHT;

	SDL_SetRenderDrawColor(main_renderer, 0, 0, 0, 255);

	while (1) {
		
		paintLevel(game_maps[current_map], map_elements,x,y);

		while (SDL_PollEvent(&ev) != 0) {
			if (ev.type == SDL_QUIT) {
				out = 1;
			}
			else if (ev.type == SDL_KEYDOWN) {
				switch (ev.key.keysym.sym) {
				case SDLK_UP:
					if (x == 0) x = SCREEN_HEIGHT / UNIT - 2;
					else x-=2;	break;

				case SDLK_DOWN:
					if (x == SCREEN_HEIGHT / UNIT - 2) x = 0;
					else x+=2; break;

				case SDLK_LEFT:
					if (y == 0) y = SCREEN_WIDTH / UNIT - 2;
					else y-=2;
					break;

				case SDLK_RIGHT:
					if (y == SCREEN_WIDTH / UNIT-2) y =  0;
					else y+=2;
					break;

				case SDLK_1:
					game_maps[current_map][x][y] = BRICK;
					game_maps[current_map][x+1][y] = BRICK; 
					game_maps[current_map][x][y+1] = BRICK; 
					game_maps[current_map][x+1][y+1] = BRICK; break;

				
				case SDLK_2:
					game_maps[current_map][x][y] = WATER;
					game_maps[current_map][x + 1][y] = WATER;
					game_maps[current_map][x][y + 1] = WATER;
					game_maps[current_map][x + 1][y + 1] = WATER; break;
				case SDLK_3:
					game_maps[current_map][x][y] = STEEL;
					game_maps[current_map][x + 1][y] = STEEL;
					game_maps[current_map][x][y + 1] = STEEL;
					game_maps[current_map][x + 1][y + 1] = STEEL; break;
					break;
				case SDLK_4:
					game_maps[current_map][x][y] = BLACK;
					game_maps[current_map][x + 1][y] = BLACK;
					game_maps[current_map][x][y + 1] = BLACK;
					game_maps[current_map][x + 1][y + 1] = BLACK; break;
				case SDLK_SPACE: current_map++; break;

				case SDLK_ESCAPE: out = 1; break;

				case SDLK_m:
					if (Mix_PlayingMusic() == 0)
						//Play the music
						Mix_PlayMusic(gMusic, -1);
					//If music is being played
					else
					{
						//If the music is paused
						if (Mix_PausedMusic() == 1)
							Mix_ResumeMusic();
						//If the music is playing
						else
							Mix_PauseMusic();
					}
					break;
				}
			}
			if (current_map > MAX_LEVELS) {
				game_maps[current_map] = malloc(SCREEN_HEIGHT / UNIT * sizeof(int*));
				for ( i = 0; i < SCREEN_HEIGHT / UNIT; i++)
					game_maps[current_map][i] = calloc(SCREEN_WIDTH / UNIT, sizeof(int));		
				MAX_LEVELS++;
				x = 0;
				y = 0;
			}


		}
		if (out) break;
	}

	cleanSpots(game_maps);
	upisi(game_maps);
	SDL_SetRenderDrawColor(main_renderer, PRIM_COLOR_R, PRIM_COLOR_G, PRIM_COLOR_B, 255);
	
}

void paintLevel(int **map, SDL_Texture* map_elements[TOTAL_ELEMENTS],int x,int y) {
	int i, j;
	SDL_Rect field;

	field.h = UNIT;
	field.w = UNIT;
	SDL_RenderClear(main_renderer);

	for(i=0;i<SCREEN_HEIGHT/(UNIT);i++)
		for(j=0;j<SCREEN_WIDTH/(UNIT);j++)
			if (map[i][j]) {
				field.y = i*UNIT;
				field.x = j*UNIT;
				SDL_RenderCopy(main_renderer, map_elements[map[i][j]], NULL, &field);
			}
	field.h = 2 * UNIT;
	field.w = 2 * UNIT;
	field.y = x  * UNIT;
	field.x = y  * UNIT;
	SDL_RenderCopy(main_renderer, map_elements[BLACK], NULL, &field);

	SDL_RenderPresent(main_renderer);
}

