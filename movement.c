#include <SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include <stdio.h>
#include <string.h>
#include "functions.h"
#include<stdlib.h>
#include<math.h>
#include<time.h>

extern int shield;

extern SDL_Color PRIM_COLOR = { PRIM_COLOR_R, PRIM_COLOR_G, PRIM_COLOR_B };
extern SDL_Color BACK_COLOR1 = { BACK_COLOR_R, BACK_COLOR_G, BACK_COLOR_B };




void make_the_move(int next_moves[MAX_BOTS + 1], struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int **map,int skip) {

	
	for (int i = 0; i < MAX_BOTS + 1; i++) {
		if (!skip || !i) {
			switch (next_moves[i]) {
			case UP: 

				if (canGoUp(tanks[i].x, tanks[i].y, i, 1, tanks, bullets, map) == IS_EMPTY) {
				tanks[i].y -= UNIT;
			} break;
			case DOWN:

				if (canGoDown(tanks[i].x, tanks[i].y, i, 1, tanks, bullets, map) == IS_EMPTY) {
				tanks[i].y += UNIT;
			} break;
			case LEFT:
				
				if (canGoLeft(tanks[i].x, tanks[i].y, i, 1, tanks, bullets, map) == IS_EMPTY) {
				tanks[i].x -= UNIT;
			} break;
			case RIGHT:
			
				if ((canGoRight(tanks[i].x, tanks[i].y, i, 1, tanks, bullets, map)) == IS_EMPTY) {
				tanks[i].x += UNIT;
			} break;

			case SHOOT: if (bullets[i].x < 0) {
				switch (tanks[i].direction) {
				case UP: bullets[i].x = tanks[i].x + UNIT;
					bullets[i].y = tanks[i].y;
					bullets[i].direction = UP;
					break;

				case DOWN:bullets[i].x = tanks[i].x + UNIT;
					bullets[i].y = tanks[i].y + 3 * UNIT;
					bullets[i].direction = DOWN;
					break;

				case LEFT:bullets[i].x = tanks[i].x;;
					bullets[i].y = tanks[i].y + UNIT;
					bullets[i].direction = LEFT;
					break;

				case RIGHT:bullets[i].x = tanks[i].x + 3 * UNIT;
					bullets[i].y = tanks[i].y + UNIT;
					bullets[i].direction = RIGHT;
					break;

				}
			}

			}
		}
	}

}
//ili mzd bolje da samo za jednog, pa da je pozivam vise puta

//kreira random powerup na random mestu
void makePower(struct PowerUp* power) {
	
	power->type = rand() % powerup_num;
	power->x = (int)((rand() / (double)RAND_MAX)*((SCREEN_WIDTH-20) / UNIT)*UNIT);
	power->y = (int)((rand() / (double)RAND_MAX)*((SCREEN_HEIGHT-20) / UNIT)*UNIT);
	
}
//brise powerup
void delPower(struct PowerUp* power) {
	power->type = -1;
	power->x = -32;
	power->y = -32;
}
//powerup effects
void makeEffect(int type,int*lives,struct PowerUp*power,struct Tank tanks[MAX_BOTS+1],int ***maps,int lvl,int*freeze,int*shovels) {
	score += 1000;
	switch (type) {
	case life: (*lives)++; if (sound)
		Mix_PlayChannel(-1, pow_life, 0); break;
	case bomb: if (sound)
		Mix_PlayChannel(-1, pow_bomb, 0);
		for (int i = 1; i < MAX_BOTS + 1; i++) tanks[i].x = -32; break;
	case shovel: *shovels = 1; updateWall(STEEL, maps[lvl]); if (sound)
		Mix_PlayChannel(-1, pow_shovel, 0); break;
	case star: if (tanks[0].strenght < 3) tanks[0].strenght++; if (sound)
		Mix_PlayChannel(-1, pow_star, 0); break;
	case Clock: *freeze = 1; if (sound)
		Mix_PlayChannel(-1, pow_clock, 0); break;
	case Shield: shield = 1; if (sound)
		Mix_PlayChannel(-1, pow_shield, 0);
	}
	delPower(power);
}
//pokupio powerup
void takePowerup(int next_moves[MAX_BOTS + 1], struct Tank tanks[MAX_BOTS + 1],struct PowerUp*power,int*lives,int***maps, int lvl,int*freeze,int*shovel) {
	
	switch (next_moves[0]) {
	case UP:	
		if(abs(power->x-tanks[0].x)<TANK_HEIGHT*UNIT)
			if (abs(tanks[0].y - power->y)<TANK_HEIGHT*UNIT ) makeEffect(power->type, lives, power,tanks,maps,lvl,freeze,shovel); break;
	
	case DOWN:	if (abs(power->x - tanks[0].x)<TANK_HEIGHT*UNIT)
					if (abs(tanks[0].y - power->y)<TANK_HEIGHT*UNIT) makeEffect(power->type, lives, power, tanks, maps, lvl, freeze, shovel); break;

	
	case LEFT:	if (abs(power->x - tanks[0].x)<TANK_HEIGHT*UNIT)
						if (abs(tanks[0].y - power->y)<TANK_HEIGHT*UNIT)makeEffect(power->type, lives, power, tanks, maps, lvl, freeze, shovel); break;

	
	case RIGHT:	if (abs(power->x - tanks[0].x)<TANK_HEIGHT*UNIT)
		if (abs(tanks[0].y - power->y)<TANK_HEIGHT*UNIT) makeEffect(power->type, lives, power, tanks, maps, lvl, freeze, shovel); break;
	}
}

destroyBullets(struct Bullet bullets[MAX_BOTS + 1]) {
	for (int i = 0; i <= MAX_BOTS; i++)
		bullets[i].x = -32;
}

//Pomera metke na mapi i proverava da li udaraju u nesto ili ubijaju nekog
void moveTheBullets(struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], struct PowerUp* power,int **map, int *done, int cordx,int cordy,int *lives,int start,int range) {
	int i, j;
	const char*died = "You died";
	TTF_Font *gFont = TTF_OpenFont("gfx/Capture_it.ttf", 28);
	
	for (i = start; i < range; i++) {
		if (bullets[i].x >= 0) {
			switch (bullets[i].direction) {
			case UP:
				switch (canGoUp(bullets[i].x, bullets[i].y, i, 0, tanks, bullets, map)) {
				case IS_EMPTY:case IS_WATER:if(isThereBullet(UP,i,bullets)) bullets[i].y -= UNIT; break;

				case IS_BRICK: for (j = bullets[i].x / UNIT - 1; j < bullets[i].x / UNIT + 3; j++)
					if (map[bullets[i].y / UNIT - 1][j] == BRICK) map[bullets[i].y / UNIT - 1][j] = IS_EMPTY;
					bullets[i].x = -32; break;

				case IS_STEEL:if (tanks[i].strenght > 2) {
					for (j = bullets[i].x / UNIT - 1; j < bullets[i].x / UNIT + 3; j++)
						if ((map[bullets[i].y / UNIT - 1][j] == BRICK) || (map[bullets[i].y / UNIT - 1][j] == STEEL)) map[bullets[i].y / UNIT - 1][j] = IS_EMPTY;
				
				}
							  bullets[i].x = -32; break;
				case IS_AT_BOTTOM: case IS_AT_TOP: case IS_LEFT: case IS_RIGHT:bullets[i].x = -32; break;
				case IS_TANK: if (i == 0) {
					if (tanks[tank_index].strenght) {
						tanks[tank_index].strenght--;
						makePower(power);//funkcija koja prai powerup
					}
					else  tanks[tank_index].x = -32;
					bullets[i].x = -32;
					score += 500;
				}
							  else {
								  if (i != 0 && tank_index == 0 && shield == 0) {
									  (*lives)--;
									  if (sound)
										  Mix_PlayChannel(-1, die, 0);
									  if (*lives) {
										  tanks[0].x = cordx;
										  tanks[0].y = cordy;
										  tanks[0].direction = UP;
										  tanks[0].strenght = 0;
									  }
									  else *done = 1;
									  writeText(died, gFont, SCREEN_WIDTH / 2 - 10 * strlen(died), SCREEN_HEIGHT / 2 - 50, 70, 170, PRIM_COLOR, BACK_COLOR1);
									  SDL_RenderPresent(main_renderer);
									  SDL_Delay(2000);
									  destroyBullets(bullets);
								  }
								  else if (i != 0 && tank_index == 0 && shield > 0) {
									  shield = 0;
									  bullets[i].x = -32;
								  }
								  else if (i != 0 && tank_index != 0) 
									  bullets[i].x = -32;
								  
							  }
				}break;

			case DOWN:
				switch (canGoDown(bullets[i].x, bullets[i].y, i, 0, tanks, bullets, map)) {
				case IS_EMPTY:case IS_WATER: if (isThereBullet(DOWN, i, bullets))bullets[i].y += UNIT; break;

				case IS_BRICK: for (j = bullets[i].x / UNIT - 1; j < bullets[i].x / UNIT + 3; j++)
					if (map[bullets[i].y / UNIT + 1][j] == BRICK) map[bullets[i].y / UNIT + 1][j] = IS_EMPTY;
					bullets[i].x = -32; break;
			
				case IS_STEEL:if (tanks[i].strenght > 2) {
					for (j = bullets[i].x / UNIT - 1; j < bullets[i].x / UNIT + 3; j++)
						if ((map[bullets[i].y / UNIT + 1][j] == BRICK) || (map[bullets[i].y / UNIT + 1][j] == STEEL)) map[bullets[i].y / UNIT + 1][j] = IS_EMPTY;
				}
							  bullets[i].x = -32; break;
				case IS_AT_BOTTOM: case IS_AT_TOP: case IS_LEFT: case IS_RIGHT:bullets[i].x = -32; break;
				case IS_TANK: if (i == 0) {
					if (tanks[tank_index].strenght) {
						tanks[tank_index].strenght--;
						makePower(power);//funkcija koja prai powerup
					}
					else tanks[tank_index].x = -32;
					bullets[i].x = -32;
					score += 500;
				}
							  else {
								  if (i != 0 && tank_index == 0 && shield == 0) {
									  (*lives)--;
									  if (sound)
										  Mix_PlayChannel(-1, die, 0);
									  if (*lives) {
										  tanks[0].x = cordx;
										  tanks[0].y = cordy;
										  tanks[0].direction = UP;
										  tanks[0].strenght = 0;
									  }
									  else *done = 1;
									  writeText(died, gFont, SCREEN_WIDTH / 2 - 10 * strlen(died), SCREEN_HEIGHT / 2 - 50, 70, 170, PRIM_COLOR, BACK_COLOR1); 
									  SDL_RenderPresent(main_renderer);
									  SDL_Delay(2000);
									  destroyBullets(bullets);
								  }
								  else if (i != 0 && tank_index == 0 && shield > 0) {
									  shield = 0;
									  bullets[i].x = -32;
								  }
								  else if (i != 0 && tank_index != 0)
									  bullets[i].x = -32;
							  }

				}break;

			case LEFT:
				switch (canGoLeft(bullets[i].x, bullets[i].y, i, 0, tanks, bullets, map)) {
				case IS_EMPTY:case IS_WATER: if (isThereBullet(LEFT, i, bullets))bullets[i].x -= UNIT; break;

				case IS_BRICK: for (j = bullets[i].y / UNIT - 1; j < bullets[i].y / UNIT + 3; j++)
					if (map[j][bullets[i].x / UNIT - 1] == BRICK) map[j][bullets[i].x / UNIT - 1] = IS_EMPTY;
					bullets[i].x = -32; break;
				//case IS_EAGLE: *done = 1; break;
				case IS_STEEL:if (tanks[i].strenght > 2) {
					for (j = bullets[i].y / UNIT - 1; j < bullets[i].y / UNIT + 3; j++)
						if ((map[j][bullets[i].x / UNIT - 1] == BRICK) || (map[j][bullets[i].x / UNIT - 1] == STEEL)) map[j][bullets[i].x / UNIT - 1] = IS_EMPTY;
				}
							  bullets[i].x = -32; break;
				case IS_AT_BOTTOM: case IS_AT_TOP: case IS_LEFT: case IS_RIGHT:bullets[i].x = -32; break;
				case IS_TANK: if (i == 0) {
					if (tanks[tank_index].strenght) {
						tanks[tank_index].strenght--;
						makePower(power);//funkcija koja prai powerup
					}
					else  tanks[tank_index].x = -32;
					bullets[i].x = -32;
					score += 500;
				}
							  else {
								  if (i != 0 && tank_index == 0 && shield == 0) {
									  (*lives)--;
									  if (sound)
										  Mix_PlayChannel(-1, die, 0);
									  if (*lives) {
										  tanks[0].x = cordx;
										  tanks[0].y = cordy;
										  tanks[0].direction = UP;
										  tanks[0].strenght = 0;
									  }
									  else *done = 1;
									  writeText(died, gFont, SCREEN_WIDTH / 2 - 10 * strlen(died), SCREEN_HEIGHT / 2 - 50, 70, 170, PRIM_COLOR, BACK_COLOR1);
									  SDL_RenderPresent(main_renderer);
									  SDL_Delay(2000);
									  destroyBullets(bullets);
								  }

								  else if (i != 0 && tank_index == 0 && shield > 0) {
									  shield = 0;
									  bullets[i].x = -32;
								  }
								  else if (i != 0 && tank_index != 0)
									  bullets[i].x = -32;
							  }

				}break;


			case RIGHT:
				switch (canGoRight(bullets[i].x, bullets[i].y, i, 0, tanks, bullets, map)) {
				case IS_EMPTY:case IS_WATER:if (isThereBullet(RIGHT, i, bullets))bullets[i].x += UNIT; break;

				case IS_BRICK: for (j = bullets[i].y / UNIT - 1; j < bullets[i].y / UNIT + 3; j++)

					if (map[j][bullets[i].x / UNIT + 1] == BRICK) map[j][bullets[i].x / UNIT + 1] = IS_EMPTY;
					bullets[i].x = -32; break;
			//	case IS_EAGLE: *done = 1; break;
				case IS_STEEL:if (tanks[i].strenght > 2) {
					for (j = bullets[i].y / UNIT - 1; j < bullets[i].y / UNIT + 3; j++)
						if ((map[j][bullets[i].x / UNIT + 1] == BRICK) || (map[j][bullets[i].x / UNIT + 1] == STEEL)) map[j][bullets[i].x / UNIT + 1] = IS_EMPTY;
				}
							  bullets[i].x = -32; break;
				case IS_AT_BOTTOM: case IS_AT_TOP: case IS_LEFT: case IS_RIGHT:bullets[i].x = -32; break;
				case IS_TANK: if (i == 0) {
					if (tanks[tank_index].strenght) {
						tanks[tank_index].strenght--;
						makePower(power);//funkcija koja prai powerup
					}
					else  tanks[tank_index].x = -32;
					bullets[i].x = -32;
					score += 500;
				}
							  else {
								  if (i != 0 && tank_index == 0 && shield == 0) {
									  (*lives)--;
									  if (sound)
										  Mix_PlayChannel(-1, die, 0);
									  if (*lives) {
										  tanks[0].x = cordx;
										  tanks[0].y = cordy;
										  tanks[0].direction = UP;
										  tanks[0].strenght = 0;
									  }
									  else *done = 1;
									  writeText(died, gFont, SCREEN_WIDTH / 2 - 10 * strlen(died), SCREEN_HEIGHT / 2 - 50, 70, 170, PRIM_COLOR, BACK_COLOR1);
									  SDL_RenderPresent(main_renderer);
									  SDL_Delay(2000);
									  destroyBullets(bullets);
								  }

								  else if (i != 0 && tank_index == 0 && shield > 0) {
									  shield = 0;
									  bullets[i].x = -32;
								  }
								  else if (i != 0 && tank_index != 0)
									  bullets[i].x = -32;
							  }

				}break;



			}
		}
	}
	TTF_CloseFont(gFont);
	gFont = NULL;
}

/* f-je koje proveravaju da li je moguce kretanje u tom smeru za metke i za tenkove (a pozivaju ih moveBullets i moveTanks)
vracaju razlicit integer u zavisnosti od toga sta je na putu
*/
int canGoUp(int x, int y, int index, int bullet_or_tank /* 0-bullet, 1-tank*/, struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int** map) {
	int j, i, k;
	if (y / UNIT > 0) {
		j = y / UNIT - 1;
		for (i = x / UNIT; i < x / UNIT + TANK_WIDTH; i++) {
			if ((map[j][i] == WATER) && (bullet_or_tank)) return IS_WATER;
			if ((map[j][i] == BRICK)) {
				if (!bullet_or_tank) {
					if (i == x / UNIT || i == x / UNIT + 1) return IS_BRICK;
				}
				else return IS_BRICK;
			}
			if ((map[j][i] == STEEL)) {
				if (!bullet_or_tank) {
					if (i == x / UNIT || i == x / UNIT + 1) return IS_STEEL;
				}
				else return IS_STEEL;
			}
			for (k = 0; k < MAX_BOTS + 1; k++) {
				if (index != k) {													
					if ((tanks[k].y / UNIT <= j) && (tanks[k].y / UNIT + TANK_WIDTH > j) && (tanks[k].x / UNIT + TANK_WIDTH > i) && (tanks[k].x / UNIT <= i)) {
						if ((!bullet_or_tank) && (i == x / UNIT || i == x / UNIT + 1)) { tank_index = k; return IS_TANK; }
						if (bullet_or_tank) { tank_index = k; return IS_TANK; }
					}
				}
			}
		}
		return IS_EMPTY;
	}
	else return IS_AT_TOP;
}

int canGoDown(int x, int y, int index, int bullet_or_tank /* 0-bullet, 1-tank*/, struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int** map) {
	int j, i, k;
	int size;
	if (bullet_or_tank) size = TANK_WIDTH;
	else size = 1;

	if ((y / UNIT + size < SCREEN_HEIGHT / UNIT)&&y>=0) {
		j = y / UNIT + size;
		for (i = x / UNIT; i < x / UNIT + TANK_WIDTH; i++) {
			if ((map[j][i] == WATER) && (bullet_or_tank)) return IS_WATER;
			if ((map[j][i] == BRICK)) {
				if (!bullet_or_tank) {
					if (i == x / UNIT || i == x / UNIT + 1) return IS_BRICK;
				}
				else return IS_BRICK;
			}

			if (isItEagle(map,j,i) && bullet_or_tank) return IS_EAGLE;


			if ((map[j][i] == STEEL)) {
				if (!bullet_or_tank) {
					if (i == x / UNIT || i == x / UNIT + 1) return IS_STEEL;
				}
				else return IS_STEEL;
			}
			for (k = 0; k < MAX_BOTS + 1; k++) {
				if (index != k) {
					if ((tanks[k].y / UNIT <= j) && (tanks[k].y / UNIT + TANK_WIDTH > j) && (tanks[k].x / UNIT + TANK_WIDTH > i) && (tanks[k].x / UNIT <= i)) {
						if ((!bullet_or_tank) && (i == x / UNIT || i == x / UNIT + 1)) { tank_index = k; return IS_TANK; }
						if (bullet_or_tank) { tank_index = k; return IS_TANK; }
					}
				}
			}
		}
		return IS_EMPTY;
	}
	else return IS_AT_BOTTOM;
}

int canGoLeft(int x, int y, int index, int bullet_or_tank /* 0-bullet, 1-tank*/, struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int** map) {
	int j, i, k;
	int step;
	if (!bullet_or_tank) step = 1;
	else step = 0;

	if (x / UNIT > 0) {
		j = x / UNIT - 1;
		for (i = y / UNIT - step; i < y / UNIT - step + TANK_HEIGHT; i++) {
			if ((map[i][j] == WATER) && (bullet_or_tank)) return IS_WATER;
			if ((map[i][j] == BRICK)) {
				if (!bullet_or_tank) {
					if (i == y / UNIT || i == y / UNIT + 1) return IS_BRICK;
				}
				else return IS_BRICK;
			}
			if (isItEagle(map, i, j) && bullet_or_tank) return IS_EAGLE;


			if ((map[i][j] == STEEL)) {
				if (!bullet_or_tank) {
					if (i == y / UNIT || i == y / UNIT + 1) return IS_STEEL;
				}
				else return IS_STEEL;
			}
			for (k = 0; k < MAX_BOTS + 1; k++) {
				if ((tanks[k].x / UNIT <= j) && (tanks[k].x / UNIT + TANK_HEIGHT > j) && (tanks[k].y / UNIT + TANK_WIDTH > i) && (tanks[k].y / UNIT <= i)) {
					if ((!bullet_or_tank) && (i == y / UNIT || i == y / UNIT + 1)) { tank_index = k; return IS_TANK; }
					if (bullet_or_tank) { tank_index = k; return IS_TANK; }
				}
			}
		}
		return IS_EMPTY;
	}
	else return IS_LEFT;
}

int canGoRight(int x, int y, int index, int bullet_or_tank /* 0-bullet, 1-tank*/, struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int** map) {
	int j, i, k;
	int size, step;
	if (bullet_or_tank) {
		size = TANK_WIDTH; step = 0;
	}
	else {
		size = 1; step = 1;
	}

	if ((x / UNIT + size < SCREEN_WIDTH / UNIT) && x>=0) {
		j = x / UNIT + size;
		for (i = y / UNIT - step; i < y / UNIT - step + TANK_HEIGHT; i++) {
			if ((map[i][j] == WATER) && (bullet_or_tank)) return IS_WATER;
			if ((map[i][j] == BRICK)) {
				if (!bullet_or_tank) {
					if (i == y / UNIT || i == y / UNIT + 1) return IS_BRICK;
				}
				else return IS_BRICK;
			}

			if (isItEagle(map, i, j) && bullet_or_tank) return IS_EAGLE;

			if ((map[i][j] == STEEL)) {
				if (!bullet_or_tank) {
					if (i == y / UNIT || i == y / UNIT + 1) return IS_STEEL;
				}
				else return IS_STEEL;
			}

			for (k = 0; k < MAX_BOTS + 1; k++) {
				if (index != k) {
					if ((tanks[k].x / UNIT <= j) && (tanks[k].x / UNIT + TANK_HEIGHT > j) && (tanks[k].y / UNIT + TANK_WIDTH > i) && (tanks[k].y / UNIT <= i)) {
						if ((!bullet_or_tank) && (i == y / UNIT || i == y / UNIT + 1)) { tank_index = k; return IS_TANK; }
						if (bullet_or_tank) { tank_index = k; return IS_TANK; }
					}
				}
			}
		}

		return IS_EMPTY;
	}

	 return IS_RIGHT;
}

int isThereBullet(int direction, int index, struct Bullet bullets[MAX_BOTS + 1]) {
	int i;
	switch (direction) {
	case UP:	for (i = 0; i < MAX_BOTS + 1; i++) {
		if (i != index && ((bullets[i].y == bullets[index].y - UNIT)) && (bullets[i].x <= bullets[index].x + UNIT) && (bullets[i].x >= bullets[index].x - UNIT)) {
			if (((i == 0) && (index > 0)) || ((index == 0) && (i > 0))) {
				bullets[i].x = -32;
				bullets[index].x = -32;
				return 0;
			}
		}
	}
				return 1;

	case DOWN:for (i = 0; i < MAX_BOTS + 1; i++) {
		if (i != index && ((bullets[i].y == bullets[index].y + UNIT)) && (bullets[i].x <= bullets[index].x + UNIT) && (bullets[i].x >= bullets[index].x - UNIT)) {
			if (((i == 0) && (index > 0)) || ((index == 0) && (i > 0))) {
				bullets[i].x = -32;
				bullets[index].x = -32;
				return 0;
			}
		}
	}
			  return 1;

	case LEFT:for (i = 0; i < MAX_BOTS + 1; i++) {
		if (i != index && ((bullets[i].x == bullets[index].x - UNIT)) && (bullets[i].y <= bullets[index].y + UNIT) && (bullets[i].y >= bullets[index].y - UNIT)) {
			if (((i == 0) && (index > 0)) || ((index == 0) && (i > 0))) {
				bullets[i].x = -32;
				bullets[index].x = -32;
				return 0;
			}
		}
	}
			  return 1;

	case RIGHT: for (i = 0; i < MAX_BOTS + 1; i++) {
		if (i != index && ((bullets[i].x == bullets[index].x + UNIT)) && (bullets[i].y <= bullets[index].y + UNIT) && (bullets[i].y >= bullets[index].y - UNIT)) {
			if (((i == 0) && (index > 0)) || ((index == 0) && (i > 0))) {
				bullets[i].x = -32;
				bullets[index].x = -32;
				return 0;
			}
		}
	}
	default:	return 1;

	}
	
}

int hitEagle(struct Bullet bullets[MAX_BOTS + 1])
{
	int px, py;
	int i, pticay = SCREEN_HEIGHT / UNIT - 4, pticax = SCREEN_WIDTH / UNIT / 2 - 2;
	for (i = 0; i <= MAX_BOTS; i++)
	{
		if (bullets[i].x < 0) continue;
		if (bullets[i].direction == UP) continue;
		switch (bullets[i].direction) {
		case DOWN: px = bullets[i].x / UNIT + 1; 
			py = bullets[i].y / UNIT; break;
		case LEFT:case RIGHT: px = bullets[i].x / UNIT;
			py = bullets[i].y / UNIT + 1; break;
		
		}

		if (bullets[i].y / UNIT >= pticay && bullets[i].x / UNIT >= pticax && bullets[i].x / UNIT < pticax + 4) return 1;
		
		if (py >= pticay && px  >= pticax && px  < pticax + 4) return 1;

	}

	return 0;
}

int isItEagle(int**map, int i, int j) {

	if ((i >= (SCREEN_HEIGHT / UNIT) - TANK_HEIGHT) && (j >= (SCREEN_WIDTH / UNIT)/2 - 2) && (j < (SCREEN_WIDTH / UNIT)/2 + 2))
		return 1;

	return 0;
}