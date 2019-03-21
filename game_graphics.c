#include <SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include <stdio.h>
#include<string.h>
#include"functions.h"
#include<stdlib.h>

extern int TEXT_HEIGHT_DIV = 12;

extern SDL_Color TEXT1_COLOR = { TEXT1_COLOR_R, TEXT1_COLOR_G, TEXT1_COLOR_B };
extern SDL_Color TEXT2_COLOR = { TEXT2_COLOR_R, TEXT2_COLOR_G, TEXT2_COLOR_B };

extern SDL_Color PRIM_COLOR1 = { PRIM_COLOR_R, PRIM_COLOR_G, PRIM_COLOR_B };
extern SDL_Color BACK_COLOR = { BACK_COLOR_R, BACK_COLOR_G, BACK_COLOR_B };



void writeText(const char *text, TTF_Font *g_font, int x, int y, int height, int width, SDL_Color front_color, SDL_Color back_color)
{

	SDL_Surface* surface = NULL;
	SDL_Texture* texture;
	surface = TTF_RenderText_Solid(g_font, text, front_color, back_color);

	texture = SDL_CreateTextureFromSurface(main_renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;

	SDL_Rect cord;
	cord.x = x;
	cord.y = y;
	cord.h = height;
	cord.w = width;

	SDL_RenderCopy(main_renderer, texture, NULL, &cord);
	SDL_DestroyTexture(texture);
}


//Crta meni (u zavisnosti od trenutnog stanja( koje polje je aktivno ))
//svaka vrednost option ima svoju poziciju na ekranu u zavisnosti od koje printa nesto (zvezdu)
void paintMenu( int choice) {
	int TEXT_HEIGHT = SCREEN_HEIGHT / TEXT_HEIGHT_DIV;
	int x, y, space, i;

	

	const char* MENU = "MENU";
	const char* PLAY = "PLAY";
	const char* SETTINGS = "SETTI NGS";
	const char* HIGHSCORES = "HIGH SCORES";
	const char* INFO = "INFO";
	const char* EXIT = "EXIT";
	const char* ARROW = "->";

	TTF_Font *gFont = TTF_OpenFont("gfx/Capture_it.ttf", 28);
	if (gFont == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());

	}

	 x = SCREEN_WIDTH / 2;
	 y = SCREEN_HEIGHT / 2 - SCREEN_HEIGHT / 10;
	 space = SCREEN_HEIGHT / 10;
	 i = 0;


	 writeText(MENU, gFont, x, y + space*i++, TEXT_HEIGHT, strlen(MENU) * 25, TEXT1_COLOR, BACK_COLOR);
	 writeText(PLAY, gFont, x, y + space*i++, TEXT_HEIGHT, strlen(PLAY) * 25, TEXT1_COLOR, BACK_COLOR);
	 writeText(SETTINGS, gFont, x, y + space*i++, TEXT_HEIGHT, strlen(SETTINGS) * 25, TEXT1_COLOR, BACK_COLOR);
	 writeText(HIGHSCORES, gFont, x, y + space*i++, TEXT_HEIGHT, strlen(HIGHSCORES) * 25, TEXT1_COLOR, BACK_COLOR);
	 writeText(INFO, gFont, x, y + space*i++, TEXT_HEIGHT, strlen(INFO) * 25, TEXT1_COLOR, BACK_COLOR);
	 writeText(EXIT, gFont, x, y + space*i++, TEXT_HEIGHT, strlen(EXIT) * 25, TEXT1_COLOR, BACK_COLOR);
	 writeText(ARROW, gFont, x / 2, (choice + 1) *(space)+y, TEXT_HEIGHT, strlen(ARROW) * 25, TEXT1_COLOR, BACK_COLOR);


	SDL_RenderPresent(main_renderer);
	SDL_RenderClear(main_renderer);
	TTF_CloseFont(gFont);
	
	gFont = NULL;

}


char *createHighScoreText(char *user, int score)
{
	char highScore[40] = " .", spaces[26], score_str[10];
	int i;
	strcat(highScore, user);
	_itoa(score, score_str, 10);
	for (i = 0; i < (25 - strlen(score_str) - strlen(user)); i++)
	{
		spaces[i] = ' ';
	}
	spaces[i] = '\0';
	strcat(highScore, spaces);
	strcat(highScore, score_str);

	return highScore;
}

void paintHighScores() {
	int TEXT_HEIGHT = SCREEN_HEIGHT / TEXT_HEIGHT_DIV;
	int x, y, space, i;
	char i_str[200], str2[2];

	const char* HIGHSCORES = "HIGH SCORES";
	char* TEMP; //= " .RatkoAMAN       1000000";//len 26
	char* user = "RatkoAMan";
	int score = 1234567;
	SCORES* high_scores = starting_scores();

	TTF_Font *gFont = TTF_OpenFont("gfx/LucidaBrightDemiBold.ttf", 28);
	if (gFont == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());

	}
	x = SCREEN_WIDTH / 20;
	y = SCREEN_HEIGHT / 10;
	space = SCREEN_HEIGHT / 11;

	writeText(HIGHSCORES, gFont, SCREEN_WIDTH / 2 - strlen(HIGHSCORES) * SCREEN_HEIGHT / 48, y / 4, TEXT_HEIGHT, strlen(HIGHSCORES) * SCREEN_HEIGHT / 24, TEXT1_COLOR, BACK_COLOR);
	sort_scores(high_scores);

	for (i = 0; i < 10; i++)
	{
		TEMP = createHighScoreText(high_scores[i].username, high_scores[i].score);

		if (i < 9) {
			i_str[0] = '0' + i + 1;
			i_str[1] = '\0';
			strcat(i_str, TEMP);
			writeText(i_str, gFont, x, y / 4 + space*(i + 1), TEXT_HEIGHT, strlen(i_str) * SCREEN_HEIGHT / 24, TEXT1_COLOR, BACK_COLOR);
		}
		else
		{
			i_str[0] = '1';
			i_str[1] = '\0';
			str2[0] = '0';
			str2[1] = '\0';
			strcat(i_str, str2);
			strcat(i_str, TEMP);
			writeText(i_str, gFont, x - SCREEN_HEIGHT / 24, y / 4 + space*(i + 1), TEXT_HEIGHT, strlen(i_str) * SCREEN_HEIGHT / 24, TEXT1_COLOR, BACK_COLOR);
		}


	}
	SDL_RenderPresent(main_renderer);
	SDL_RenderClear(main_renderer);
	TTF_CloseFont(gFont);
	gFont = NULL;

}



//iscrtava mapu (vodu, cigle, celik iscrtava sa odredjenih polja matrice, a koordinate tenkova i metaka cita i crta iz prenetih struktura(njih cu kasnije definisati)
void paintMap(int*** all_maps, int current_level,int bots_left, int score, int lives,  struct PowerUp powerup, struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], SDL_Texture* player[TOTAL], SDL_Texture* bot1[TOTAL], SDL_Texture* bot_spec[TOTAL], SDL_Texture* map_elements[TOTAL_ELEMENTS]) {
	int **map = all_maps[current_level];
	int i, j;
	SDL_Rect field;
	field.w = UNIT;
	field.h = UNIT;
	char current_score[100];
	char level[5];
	char life[5];
	char *paused = "PAUSED";
	char lvl[10] = "LVL: ";
	char b_left[10];


	SDL_RenderClear(main_renderer);

	for (i = 0; i<SCREEN_HEIGHT / UNIT; i++)
		for (j = 0; j < SCREEN_WIDTH / UNIT; j++) {
			if (map[i][j]) {
				field.y = i*UNIT;
				field.x = j*UNIT;
				SDL_RenderCopy(main_renderer, map_elements[map[i][j]], NULL, &field);
			}
		}

	//crta tenkice
	for (i = 0; i < MAX_BOTS + 1; i++) {
		if (tanks[i].x >= 0) {
			field.x = tanks[i].x;
			field.y = tanks[i].y;
			field.w = TANK_WIDTH*UNIT;
			field.h = TANK_HEIGHT*UNIT;
			if (i == 0)
				SDL_RenderCopy(main_renderer, player[tanks[i].direction], NULL, &field);
			else {

				if (tanks[i].strenght)
					SDL_RenderCopy(main_renderer, bot_spec[tanks[i].direction], NULL, &field);
				else SDL_RenderCopy(main_renderer, bot1[tanks[i].direction], NULL, &field);
			}
		}
	}
	//orao
	field.x = SCREEN_WIDTH / 2 - 2 * UNIT;
	field.y = SCREEN_HEIGHT - 4 * UNIT;
	SDL_RenderCopy(main_renderer, map_elements[EAGLE], NULL, &field);

	//powerup
	if (powerup.x >= 0) {
		field.x = powerup.x;
		field.y = powerup.y;
		field.w = TANK_WIDTH*UNIT;
		field.h = TANK_HEIGHT*UNIT;
		SDL_RenderCopy(main_renderer, map_elements[LIFE + powerup.type], NULL, &field);
	}

	//meci
	for (i = 0; i < MAX_BOTS + 1; i++) {
		if (bullets[i].x >= 0) {
			field.x = bullets[i].x;
			field.y = bullets[i].y;
			switch (bullets[i].direction) {
			case UP:case DOWN:
				field.w = TANK_WIDTH / 2 * UNIT;
				field.h = UNIT;
				break;
			case LEFT:case RIGHT:
				field.w = UNIT;
				field.h = TANK_HEIGHT / 2 * UNIT;
				break;
			}
			SDL_RenderCopy(main_renderer, map_elements[BULLET], NULL, &field);
		}
	}

	//drugi user metak
	if (bullet2.x >= 0) {
		field.x = bullet2.x;
		field.y = bullet2.y;
		switch (bullet2.direction) {
		case UP:case DOWN:
			field.w = TANK_WIDTH / 2 * UNIT;
			field.h = UNIT;
			break;
		case LEFT:case RIGHT:
			field.w = UNIT;
			field.h = TANK_HEIGHT / 2 * UNIT;
			break;
		}
		SDL_RenderCopy(main_renderer, map_elements[BULLET], NULL, &field);
	}



	//mesto gde ide skor, broj zivota, svee
	field.x = 0;
	field.y = SCREEN_HEIGHT;
	field.h = 20;
	field.w = SCREEN_WIDTH;
	SDL_RenderCopy(main_renderer, map_elements[BLACK], NULL, &field);

	field.x = SCREEN_WIDTH - 5 * UNIT;
	field.y = SCREEN_HEIGHT;
	field.h = 20;
	field.w = 20;
	SDL_RenderCopy(main_renderer, player[UP], NULL, &field);

	int length = snprintf(NULL, 0, "%d", score);
	snprintf(current_score, length + 1, "%d", score);

	//SCORE,LIVES, ETC
	TTF_Font *gFont = TTF_OpenFont("gfx/Capture_it.ttf", 28);
	if (gFont == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());

	}
	length = snprintf(NULL, 0, "%d", score);
	snprintf(current_score, length + 1, "%d", score);
	writeText(current_score, gFont, 0, SCREEN_HEIGHT, 20, 20 * length, PRIM_COLOR1, BACK_COLOR);

	length = snprintf(NULL, 0, "%d", lives);
	snprintf(life, length + 1, "%d", lives);
	writeText(life, gFont, SCREEN_WIDTH - 2 * UNIT, SCREEN_HEIGHT, 20, 20, PRIM_COLOR1, BACK_COLOR);



	length = snprintf(NULL, 0, "%d", current_level);
	snprintf(level, length + 1, "%d", current_level);
	strcat(lvl, level);
	writeText(lvl, gFont, SCREEN_WIDTH / 2 + 6 * UNIT, SCREEN_HEIGHT, 20, 40 * length, PRIM_COLOR1, BACK_COLOR);

	//ostalo botova
	length = snprintf(NULL, 0, "%d", bots_left);
	snprintf(b_left, length + 1, "%d", bots_left);
	writeText(b_left, gFont, SCREEN_WIDTH / 2 - 16 * UNIT, SCREEN_HEIGHT, 20, 15 * length, PRIM_COLOR1, BACK_COLOR);
	field.x = SCREEN_WIDTH / 2 - 16 * UNIT+ 15 * length+2;
	field.y = SCREEN_HEIGHT;
	field.h = 20;
	field.w = 20;
	SDL_RenderCopy(main_renderer, bot_spec[UP], NULL, &field);


	if (pause) writeText(paused, gFont, SCREEN_WIDTH / 2 - 10 * strlen(paused), SCREEN_HEIGHT / 2 - 50, 70, 170, PRIM_COLOR1, BACK_COLOR);


	SDL_RenderPresent(main_renderer);
	TTF_CloseFont(gFont);
	gFont = NULL;
}


//Crta info
void paintInfo() {
	int TEXT_HEIGHT = SCREEN_HEIGHT / TEXT_HEIGHT_DIV;
	int x, y, space, i;



	const char* INFO = "INFO";
	const char* TEAM = "TEAM";
	const char* NIKOLA = "Nikola Kesic";
	const char* SRDJAN = "Srdjan Markovic";
	const char* NEMANJA = "Nemanja Divnic";
	const char* RATKO = "Ratko Amanovic";
	const char* MUTE1 = "PRESS M TO MUTE MUSIC";
	const char* MUTE2 = "PRESS S TO MUTE SFX";


	TTF_Font *gFont = TTF_OpenFont("gfx/Capture_it.ttf", 28);
	if (gFont == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());

	}

	x = SCREEN_WIDTH / 20;
	y = SCREEN_HEIGHT / 20;
	space = SCREEN_HEIGHT / 10;
	i = 0;


	writeText(INFO, gFont, SCREEN_WIDTH / 2 - strlen(INFO) * SCREEN_HEIGHT / 48, y + space*i++, TEXT_HEIGHT, strlen(INFO) * SCREEN_HEIGHT / 24, TEXT1_COLOR, BACK_COLOR);
	writeText(TEAM, gFont, x, y + space*i++, TEXT_HEIGHT, strlen(TEAM) * SCREEN_HEIGHT / 24, TEXT1_COLOR, BACK_COLOR);
	writeText(NIKOLA, gFont, x, y + space*i++, TEXT_HEIGHT, strlen(NIKOLA) * SCREEN_HEIGHT / 24, TEXT1_COLOR, BACK_COLOR);
	writeText(SRDJAN, gFont, x, y + space*i++, TEXT_HEIGHT, strlen(SRDJAN) * SCREEN_HEIGHT / 24, TEXT1_COLOR, BACK_COLOR);
	writeText(NEMANJA, gFont, x, y + space*i++, TEXT_HEIGHT, strlen(NEMANJA) * SCREEN_HEIGHT / 24, TEXT1_COLOR, BACK_COLOR);
	writeText(RATKO, gFont, x, y + space*i++, TEXT_HEIGHT, strlen(RATKO) * SCREEN_HEIGHT / 24, TEXT1_COLOR, BACK_COLOR);
	i++;
	writeText(MUTE1, gFont, x, y + space*i++, TEXT_HEIGHT, strlen(MUTE1) * SCREEN_HEIGHT / 24, TEXT1_COLOR, BACK_COLOR);
	writeText(MUTE2, gFont, x, y + space*i++, TEXT_HEIGHT, strlen(MUTE2) * SCREEN_HEIGHT / 24, TEXT1_COLOR, BACK_COLOR);


	SDL_RenderPresent(main_renderer);
	SDL_RenderClear(main_renderer);
	TTF_CloseFont(gFont);

	gFont = NULL;

}


//crta opcije (0 je difficulty, 1 je tezina botova)
void paintSettings(int choice) {
	int TEXT_HEIGHT = SCREEN_HEIGHT / TEXT_HEIGHT_DIV;
	int x, y, space, i;

	SDL_Surface* surface = NULL;


	const char* DIFICULTY = "DIFFICULTY";
	const char* SCREEN_SIZE = "SCREEN SIZE";
	const char* SETTINGS = "SETTI NGS";
	const char* LEVEL_EDITOR = "LEVEL EDITOR";
	const char* ARROW = "->";
	const char* EASY = "EASY";
	const char* PREMEDIUM = "MEDIUM";
	const char* MEDIUM = "HARD";
	const char* HARD = "IMPOSSIBLE";
	const char* LITTLE_SCREEN = "640 X 480";
	const char* BIG_SCREEN = "800 X 640";



	TTF_Font *gFont = TTF_OpenFont("gfx/Capture_it.ttf", 28);
	if (gFont == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());

	}

	x = SCREEN_WIDTH / 2;
	y = SCREEN_HEIGHT / 10;
	space = SCREEN_HEIGHT / 4;
	i = 0;
	writeText(SETTINGS, gFont, x, y + space * i++, TEXT_HEIGHT, strlen(SETTINGS) * 25, TEXT1_COLOR, BACK_COLOR);
	writeText(DIFICULTY, gFont, x, y + space * i, TEXT_HEIGHT, strlen(DIFICULTY) * 25, TEXT1_COLOR, BACK_COLOR);

	switch (DIFFICULTY)
	{
	case 1:
		writeText(EASY, gFont, x, y + space* (i++ + 0.3), TEXT_HEIGHT, strlen(EASY) * SCREEN_HEIGHT / 24, TEXT2_COLOR, BACK_COLOR);
		break;
	case 2:
		writeText(PREMEDIUM, gFont, x, y + space* (i++ + 0.3), TEXT_HEIGHT, strlen(PREMEDIUM) * SCREEN_HEIGHT / 24, TEXT2_COLOR, BACK_COLOR);
		break;
	case 3:
		writeText(MEDIUM, gFont, x, y + space* (i++ + 0.3), TEXT_HEIGHT, strlen(MEDIUM) * SCREEN_HEIGHT / 24, TEXT2_COLOR, BACK_COLOR);
		break;
	case 4:writeText(HARD, gFont, x, y + space* (i++ + 0.3), TEXT_HEIGHT, strlen(HARD) * SCREEN_HEIGHT / 24, TEXT2_COLOR, BACK_COLOR);
		break;
	default:
		writeText(DIFICULTY, gFont, x, y + space* (i++ + 0.3), TEXT_HEIGHT, strlen(DIFICULTY) * SCREEN_HEIGHT / 24, TEXT2_COLOR, BACK_COLOR);
		break;

	}


	writeText(SCREEN_SIZE, gFont, x, y + space * i, TEXT_HEIGHT, strlen(SCREEN_SIZE) * 25, TEXT1_COLOR, BACK_COLOR);

	switch (SCREEN_HEIGHT)
	{
	case LITTLE_HEIGHT:
		writeText(LITTLE_SCREEN, gFont, x, y + space* (i++ + 0.3), TEXT_HEIGHT, strlen(SCREEN_SIZE) * SCREEN_HEIGHT / 24, TEXT2_COLOR, BACK_COLOR);
		break;
	case BIG_HEIGHT:
		writeText(BIG_SCREEN, gFont, x, y + space* (i++ + 0.3), TEXT_HEIGHT, strlen(SCREEN_SIZE) * SCREEN_HEIGHT / 24, TEXT2_COLOR, BACK_COLOR);
		break;
	default:
		writeText(SCREEN_SIZE, gFont, x, y + space* (i++ + 0.3), TEXT_HEIGHT, strlen(SCREEN_SIZE) * SCREEN_HEIGHT / 24, TEXT2_COLOR, BACK_COLOR);
		break;
	}


	writeText(LEVEL_EDITOR, gFont, x, y + space* i++, TEXT_HEIGHT, strlen(LEVEL_EDITOR) * 25, TEXT1_COLOR, BACK_COLOR);
	writeText(ARROW, gFont, x / 2, (choice + 1) *(space)+y, TEXT_HEIGHT, strlen(ARROW) * 25, TEXT1_COLOR, BACK_COLOR);

	SDL_RenderPresent(main_renderer);
	SDL_RenderClear(main_renderer);
	TTF_CloseFont(gFont);
	gFont = NULL;
}