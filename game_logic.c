#include <SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include <stdio.h>
#include <string.h>
#include "functions.h"
#include<stdlib.h>

int shield = 0;
int score;
int two_bullets;
int pause;
struct Bullet bullet2, b_help;
int sound = 1;



//Igra, main loop
int startGame(SDL_Texture* user_tank[TOTAL],SDL_Texture* bot_tank1[TOTAL],SDL_Texture* bot_tank2[TOTAL],SDL_Texture* map_elements[TOTAL_ELEMENTS]) {
//deklaracije
	struct Tank tanks[MAX_BOTS + 1];
	struct Bullet bullets[MAX_BOTS + 1];
	int player_lives = 3;
	int ***all_maps;
	int current_level = 1;
	int freeze = -1, shovel = -1;
	struct PowerUp power_up;
	const Uint8* key_state;
	SDL_Event ev;
	int done = 0;
	unsigned int time_player_speed, shoot_time,time_bot_speed, time_shovel,time_clock, time_bullet_speed, time_check,bot_create_speed,time_bullet_p_speed;
	int *next_moves=NULL;
	int exit_game = 0;
	int bots_left = MAX_BOTS; //smanjuje se svaki put kad se rodi novi bot
	int cordx, cordy; //pocetne koordinate tenka
	int f_left=1, f_right=1;
	int c_bots=0, p_bots=0;
	int i;
	int fire_rate;
	SCORES* high_scores = starting_scores();
	int move;

	//char* imena[] = { "Nikola","Srdjan","Nemanja","Ratko","Pele","Nindza","Pera","Batman","Hari","Marko" };

	two_bullets = 0;
	score = 0;
	pause = 0;
	//mesto radjanja igraca - uz zid pored orla
	cordy = SCREEN_HEIGHT - TANK_WIDTH*UNIT;
	cordx = SCREEN_WIDTH / 2 - UNIT*2 * TANK_WIDTH;
	tanks[0].strenght = 0;

	//uzimanje int*** pokazivaca na nivoe
	if (game_maps)
		dealok(game_maps);
	game_maps = NULL;
	game_maps = getMaps2(game_maps);
	all_maps = game_maps;
	change_screen = SCREEN_HEIGHT;


	//resetovanje nizova tenkica, metaka i powerupova
	power_up.type = -1;
	power_up.x = power_up.y = -32;
	emptyStructs(tanks, bullets,cordx,cordy,&bullet2);
	shield = 0;

	//crni background
	SDL_SetRenderDrawColor(main_renderer, 0, 0, 0, 255);

	//tajmeri
	time_player_speed = 
	time_bullet_speed =
	time_bullet_p_speed=
	time_bot_speed=
	time_check=
	shoot_time=
	time_shovel=
	time_clock=
	bot_create_speed = SDL_GetTicks();

//dok traje igrica
	while (!done) {

		//ako treba da prebaci na sledeci nivo
		if (!(bots_left) && !(botsAlive(tanks))) {
			if (current_level < 99) {
				current_level++;
				if (current_level > 10) random_mapa(all_maps, current_level);
				emptyStructs(tanks, bullets, cordx, cordy,&bullet2);
				bots_left = MAX_BOTS;
				SDL_Delay(3000);
			}
			else {
				//izlaz
				done = 1;
			}
		}

		if (done) break;

		//izlaz iz programa - regulisati da je SDL_QUIT izlaz iz programa do kraja, a ESC vracanje u meni
		while (SDL_PollEvent(&ev))
			if (ev.type == SDL_QUIT) {
				exit_game = 1;
				done = 1;
			}
			else if (ev.type == SDL_KEYDOWN) {
				switch (ev.key.keysym.sym) {
				case SDLK_p:
					if (!pause) pause = 1;
					else pause = 0; break;
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
				case SDLK_s: if (sound) sound = 0; else sound = 1; break;
				}
			}
		


		if(!pause){
			//crta mapu
			paintMap(all_maps, current_level, bots_left+botsAlive(tanks),score, player_lives, power_up, tanks, bullets,user_tank, bot_tank1, bot_tank2, map_elements);
			//dodati iscrtavanje drugog metka u paintMap
			time_check = SDL_GetTicks();


			if (p_bots > (c_bots = botsAlive(tanks))) {
				bot_create_speed = time_check;
				p_bots = c_bots;
			}

			if ((bot_create_speed + (2000 * botsAlive(tanks)) + ((MAX_BOTS - bots_left) % 10) * 500<time_check) && (bots_left>0) && (areCornersClear(tanks, &f_left, &f_right))) {
				createEnemie(tanks, f_left, bots_left);
				bots_left--;
				bot_create_speed = time_check;
			}


			if (hitEagle(bullets)) {
				done = 1;
				Mix_PlayChannel(-1, gameover, 0);
				SDL_Delay(2000);
			}

		// 2 metka odjednom
		switch (tanks[0].strenght) {
		case 0: fire_rate = 1; two_bullets = 0; break;
		case 1:fire_rate = 2; two_bullets = 0; break;
		case 2:fire_rate = 1; two_bullets = 1; break;
		case 3:fire_rate = 2; two_bullets=1;break;
		}


		//kretanje metaka botova
		if (time_bullet_speed + 36 < time_check) {
			moveTheBullets(tanks, bullets, &power_up,all_maps[current_level],&done,cordx,cordy,&player_lives,1,MAX_BOTS+1);
			time_bullet_speed = time_check;
		}
		//kretanje metaka igraca
		if (time_bullet_p_speed + 32/fire_rate < time_check) {
			moveTheBullets(tanks, bullets, &power_up, all_maps[current_level], &done, cordx, cordy, &player_lives, 0, 1);
			
			//rotacija strukture (kretanje drugog metka)
			b_help.x = bullets[0].x;
			b_help.y = bullets[0].y;
			b_help.direction = bullets[0].direction;
			
			bullets[0].direction = bullet2.direction;
			bullets[0].x = bullet2.x;
			bullets[0].y = bullet2.y;

			bullet2.direction = b_help.direction;
			bullet2.x = b_help.x;
			bullet2.y = b_help.y;

			moveTheBullets(tanks, bullets, &power_up, all_maps[current_level], &done, cordx, cordy, &player_lives, 0, 1);

			b_help.x = bullets[0].x;
			b_help.y = bullets[0].y;
			b_help.direction = bullets[0].direction;

			bullets[0].direction = bullet2.direction;
			bullets[0].x = bullet2.x;
			bullets[0].y = bullet2.y;

			bullet2.direction = b_help.direction;
			bullet2.x = b_help.x;
			bullet2.y = b_help.y;



			time_bullet_p_speed = time_check;
		}


		//kretanje igraca
		if (time_player_speed + UNIT*SPEED < time_check) {

			key_state = SDL_GetKeyboardState(NULL);

			if (next_moves) free(next_moves);
			next_moves = nextMoves(tanks, bullets, all_maps[current_level]);

			for (i = 1; i < MAX_BOTS + 1; i++) {
				if ((tanks[i].x >= 0) && (next_moves[i] < 4))
					tanks[i].direction = next_moves[i];
			}
			move = 0;
			if (key_state[SDL_SCANCODE_UP]) {
				next_moves[0] = UP;
				tanks[0].direction = UP;
				move = 1;
			}
			else if (key_state[SDL_SCANCODE_DOWN]) {
				next_moves[0] = DOWN;
				tanks[0].direction = DOWN;
				move = 1;
			}
			else if (key_state[SDL_SCANCODE_LEFT]) {
				next_moves[0] = LEFT;
				tanks[0].direction = LEFT;
				move = 1;
			}
			else if (key_state[SDL_SCANCODE_RIGHT]) {
				next_moves[0] = RIGHT;
				tanks[0].direction = RIGHT;
				move = 1;
			}
			if (key_state[SDL_SCANCODE_C]) {
				if(!tanks[0].strenght)
				tanks[0].strenght = 3;
				else tanks[0].strenght = 0;
			}

			if (key_state[SDL_SCANCODE_SPACE]) {
				//ako moze da puca (jednom ili 2 puta)
				if (bullets[0].x < 0 || (bullet2.x < 0 && tanks[0].strenght>=2 && shoot_time+275<time_check)) {

					//resetuje shoot_time ako je jedan metak udario u nesto
					if (bullets[0].x < 0 || bullet2.x<0) shoot_time = time_check;


					if (bullets[0].x >= 0 && bullet2.x < 0 && tanks[0].strenght>=2) {
						b_help.x = bullets[0].x;
						b_help.y = bullets[0].y;
						b_help.direction = bullets[0].direction;

						bullets[0].direction = bullet2.direction;
						bullets[0].x = bullet2.x;
						bullets[0].y = bullet2.y;

						bullet2.direction = b_help.direction;
						bullet2.x = b_help.x;
						bullet2.y = b_help.y;
					}


					move = 1;
					next_moves[0] = SHOOT;
					if(sound)
						Mix_PlayChannel(-1, shot_fired, 0);
				}
			}
			if (!move) next_moves[0] = 7;

			if (key_state[SDL_SCANCODE_ESCAPE]) {
				done = 1;
			}

			takePowerup(next_moves, tanks, &power_up, &player_lives, all_maps, current_level, &freeze, &shovel);


			if (freeze == 1) {
				time_clock = time_check;
				freeze = 0;
			}
			if (shovel == 1) {
				time_shovel = time_check;
				shovel = 0;
			}

			if (!shovel && time_shovel + 15000 < time_check) {
				updateWall(BRICK, all_maps[current_level]);
				shovel = -1;
			}


			//za svaki tenk i svaki metak pomera ako moze, unistava ako je pogodjen itd.
			if (!freeze && (time_clock + 15000 > time_check))
				make_the_move(next_moves, tanks, bullets, all_maps[current_level], 1);

			else {

				make_the_move(next_moves, tanks, bullets, all_maps[current_level], 0);
				freeze = 5;
			}

			time_player_speed = time_check;
		}//kretanje igraca
			/* powerupovi ce biti boolovi koji ce imati svoje time check kad se jednom aktiviraju */
		
		} //if !pause
		else paintMap(all_maps, current_level,bots_left+botsAlive(tanks), score, player_lives, power_up, tanks, bullets, user_tank, bot_tank1, bot_tank2, map_elements);
	} //while !done


//HIGH SCORE
	high_scores[10].score = 0;
	sort_scores(high_scores);	
		if (score > high_scores[9].score) {
			strcpy(high_scores[9].username, newHighScore());

			for (int j = strlen(high_scores[9].username); j<9; j++) high_scores[9].username[j] = ' ';
			high_scores[9].username[9] = '\0';
			high_scores[9].score = score;
		}
		sort_scores(high_scores);
		lock_scores(high_scores);
		

	

		SDL_SetRenderDrawColor(main_renderer, PRIM_COLOR_R, PRIM_COLOR_G, PRIM_COLOR_B, 255);

	if (exit_game) 
		return 1;
	else	
		return 0;
}

void updateWall(int cime, int**map)
{
	int ty, tx, i, j, n = SCREEN_HEIGHT / UNIT, m = SCREEN_WIDTH / UNIT;
	ty = SCREEN_HEIGHT / UNIT - 6;
	tx = SCREEN_WIDTH / UNIT / 2 - 4;
	for (i = ty; i <= ty + 1; i++)
	{
		for (j = tx; j < m - tx; j++)
		{
			map[i][j] = cime;
		}
	}
	for (i = ty; i<n; i++)
	{
		for (j = tx; j<tx + 2; j++)
		{
			map[i][j] = cime;
			map[i][m-j-1] = cime;
		}
	}
}

void emptyStructs(struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1],int cordx,int cordy,struct Bullet*bul2) {
	
	for (int i = 1; i < MAX_BOTS + 1; i++) {
		tanks[i].x = tanks[i].y = -32;
		tanks[i].direction = DOWN;
		tanks[i].strenght = 0;
	}
	tanks[0].x = cordx;
	tanks[0].y = cordy;
	tanks[0].direction = UP;
	

	for (int i = 0; i < MAX_BOTS + 1; i++) {
		bullets[i].x = bullets[i].y = -32;
		bullets[i].direction = -32;
	}
	bul2->direction = -32;
	bul2->x = bul2->y = -32;
}

//radjanje botova:
void createEnemie(struct Tank tanks[MAX_BOTS + 1],int f_left,int bot_left) {
	int i;
	for (i = 1; i < MAX_BOTS + 1; i++) 
		if (tanks[i].x < 0) break;
			
	if (f_left) {
		tanks[i].x = 0;
		tanks[i].y = 0;
		tanks[i].direction = DOWN;
		if(!(bot_left%4)) tanks[i].strenght = 1;
		else tanks[i].strenght = 0;
	}
	else {
		tanks[i].x = SCREEN_WIDTH-4*UNIT;
		tanks[i].y = 0;
		tanks[i].direction = DOWN;
		if (!(bot_left % 4)) tanks[i].strenght = 1;
		else tanks[i].strenght = 0;
	}

}

//koliko zivih botova ima trenutno na mapi
int botsAlive(struct Tank tanks[MAX_BOTS + 1]) {
	int counter=0;
	for (int i = 1; i < MAX_BOTS + 1; i++) {
		if (tanks[i].x >= 0)  counter++;
	}
	return counter;
}

//da li su uglovi slobodni (moze da se rodi novi bot)
int areCornersClear(struct Tank tanks[MAX_BOTS + 1], int *flag_left, int*flag_right) {
	int i;
	*flag_left = 1;
	*flag_right = 1;
	for (i = 0; i < MAX_BOTS + 1; i++) {
		if (tanks[i].x >= 0) {
			if (tanks[i].y < 4 * UNIT) {
				if (tanks[i].x < 4 * UNIT) *flag_left = 0;
				if (tanks[i].x > SCREEN_WIDTH - 8 * UNIT) *flag_right = 0;
			}
		}
	}
	if ((*flag_left) | (*flag_right)) return 1;
	else return 0;

}

char* newHighScore() {
	SDL_Renderer* renderer = NULL;
	SDL_Window* window = NULL;
	SDL_Surface* surface = NULL;
	SDL_Texture* letter = NULL, *black = NULL, *text,*TEKST;
	const char* ime = "Enter your name (Press Esc when finished): ";
	int done = 0;
	SDL_Color color = { 255,255,255 };
	SDL_Color bg = { 0,0,0 };
	SDL_Event e;
	unsigned int time_c, time_s = 0;

	char*inputText = NULL;
	inputText = malloc(100 * sizeof(char));
	inputText[0] = '\0';

	window = SDL_CreateWindow("New High Score", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 400, 150, SDL_WINDOW_SHOWN);
	if (!window)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		exit(1);
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		exit(1);
	}
	else
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	}

	surface = IMG_Load("gfx/letter.png");
	letter= SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;

	surface = IMG_Load("gfx/black.png");
	black = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;

	TTF_Font *gFont = TTF_OpenFont("gfx/LucidaBrightDemiBold.ttf", 28);
	if (gFont == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());

	}

	surface = TTF_RenderText_Solid(gFont, ime, color, bg);
	text = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;

	SDL_Rect let;
	let.x = 10;
	let.y = 10;
	let.h = 30;
	let.w = 390;

	SDL_Rect space;
	space.x = 10;
	space.y = 60;
	space.h = 30;
	space.w = 30;

	SDL_Rect bl;
	bl.x = 0;
	bl.y = 0;
	bl.h = 200;
	bl.w = 400;

	SDL_Rect tekst;
	tekst.h = 30;
	tekst.x = 10;
	tekst.y = 90;

	SDL_Color textColor = { 0, 0, 0, 0xFF };
	

	SDL_StartTextInput();
	int renderText = 0;
	
	time_c = SDL_GetTicks();
	while (!done) {
		time_c = SDL_GetTicks();
		if (time_s + 2000 < time_c) {
			Mix_PlayChannel(-1, pow_clock, 0);
			time_s = time_c;			
		}

		SDL_RenderClear(renderer);
		space.x = strlen(inputText) * 30;
		space.y = 90;

		

		surface = TTF_RenderText_Solid(gFont, inputText, color, bg);
		TEKST = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
		surface = NULL;

		renderText = 0;
		SDL_RenderCopy(renderer, black, NULL, &bl);
		SDL_RenderCopy(renderer, text, NULL, &let);
		SDL_RenderCopy(renderer, letter, NULL, &space);
		if (strlen(inputText)) {
			tekst.w = strlen(inputText) * 30;
			SDL_RenderCopy(renderer, TEKST, NULL, &tekst);
		}

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
				done = 1;

			else if (e.type == SDL_TEXTINPUT || e.type==SDL_KEYDOWN) {
				if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_BACKSPACE && strlen(inputText) > 0)
					inputText[strlen(inputText) - 1] = '\0';
				else if(e.type==SDL_TEXTINPUT) strcat(inputText, e.text.text);
				else if (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_ESCAPE)) done = 1;
			}
			
			
		}
		SDL_DestroyTexture(TEKST);
		SDL_RenderPresent(renderer);
	}
	SDL_StopTextInput();
	SDL_DestroyTexture(text);
	TTF_CloseFont(gFont);
	gFont = NULL;
	SDL_DestroyTexture(letter);
	SDL_DestroyTexture(black);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	inputText[10] = '\0';
	return inputText;
}

