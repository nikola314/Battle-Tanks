#ifndef _BOOL
#define _BOOL 5

#define UNIT 8
#define SPEED 8
#define MAX_BOTS 20
#define IFNOT(x) {if(!(x)) success=0;}
#define player_speed UNIT*2
#define TANK_WIDTH 4
#define TANK_HEIGHT 4


#define LITTLE_HEIGHT 480
#define LITTLE_WIDTH 640
#define BIG_HEIGHT 640
#define BIG_WIDTH 800


#define PRIM_COLOR_R 255
#define PRIM_COLOR_G 215
#define PRIM_COLOR_B 0

#define TEXT1_COLOR_R 33
#define TEXT1_COLOR_G 33
#define TEXT1_COLOR_B 33

#define TEXT2_COLOR_R 255
#define TEXT2_COLOR_G 255
#define TEXT2_COLOR_B 255

#define BACK_COLOR_R 150
#define BACK_COLOR_G 150
#define BACK_COLOR_B 150


extern SDL_Window* main_window;
extern SDL_Renderer* main_renderer;
extern TTF_Font*gFont;

extern int pause;
extern int*** game_maps;
extern int MAX_LEVELS;
extern int tank_index;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int DIFFICULTY;
extern int tank_powerups[5];
extern int score;
extern two_bullets;
extern struct Bullet bullet2, b_help;
extern int sound;
extern int change_screen;

extern Mix_Music *gMusic;
extern Mix_Chunk *shot_fired;
extern Mix_Chunk *pow_bomb;
extern Mix_Chunk *pow_life;
extern Mix_Chunk *pow_shovel;
extern Mix_Chunk *pow_shield;
extern Mix_Chunk *pow_star;
extern Mix_Chunk *die;
extern Mix_Chunk *pow_clock;
extern Mix_Chunk *gameover;


enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT,
	TOTAL,
	SHOOT
	
};

enum Options {
	OPTION_PLAY,
	OPTION_SETTINGS,
	OPTION_HIGH_SCORES,
	OPTION_INFO,
	OPTION_EXIT,
	OPTION_TOTAL
};

enum Position {
	IS_EMPTY,
	IS_AT_TOP,
	IS_AT_BOTTOM,
	IS_LEFT,
	IS_RIGHT,
	IS_BRICK,
	IS_STEEL,
	IS_WATER,
	IS_TANK,
	IS_EAGLE
};

enum Power_ups {
	life,
	bomb,
	shovel,
	star,
	Clock,
	Shield,
	powerup_num
};

enum Elements_on_the_map {
	BLACK,
	BRICK,
	WATER,
	STEEL,
	EAGLE,
	BULLET,
	LIFE,
	BOMB,
	SHOVEL,
	STAR,
	CLOCK,
	SHIELD,
	TOTAL_ELEMENTS
};

struct Tank { //nulti indeks je igrac, ostalo su botovi
	int x, y; //koordinate (-1 su ako trenutno bot nije ziv)
	int direction; //smer u kom se krece
	int strenght; //da li je obicni ili specijalni bot (0 ili 1), a kod igraca oznacava koji powerup ima
};

struct Bullet {//indeks u nizu metaka odgovara indeksu u nizu tenkova (nulti je igracev)
	int x, y;//koordinate
	int direction;//smer
};

struct PowerUp {
	int type; //koji je powerup
	int x, y; //koordinate
};

typedef struct scores {
	char username[10];
	unsigned long int score;
}SCORES;



int initializeSDL(int , int );

void close_SDL();

int loadTextures( SDL_Texture* [TOTAL], SDL_Texture* [TOTAL], SDL_Texture* [TOTAL], SDL_Texture* [TOTAL_ELEMENTS]);

void freeTextures(SDL_Texture*[TOTAL], SDL_Texture* bot1[TOTAL], SDL_Texture*[TOTAL], SDL_Texture*[TOTAL_ELEMENTS]);

int startGame(SDL_Texture* user_tank[TOTAL], SDL_Texture* bot_tank1[TOTAL], SDL_Texture* bot_tank2[TOTAL], SDL_Texture* map_elements[TOTAL_ELEMENTS]);

void paintMenu(int);

int menu_gui();

void paintMap(int*** all_maps, int current_level,int, int score,int lives, struct PowerUp powerup, struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], SDL_Texture* player[TOTAL], SDL_Texture* bot1[TOTAL], SDL_Texture* bot_spec[TOTAL], SDL_Texture* map_elements[TOTAL_ELEMENTS]);

void paintSettings( int);

void settingsScreen(SDL_Texture* user_tank[TOTAL], SDL_Texture* bot_tank1[TOTAL], SDL_Texture* bot_tank2[TOTAL], SDL_Texture* map_elements[TOTAL_ELEMENTS]);

int* nextMoves(struct Tank [MAX_BOTS + 1], struct Bullet[MAX_BOTS + 1], int** );

int*** getMaps();

void emptyStructs(struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int cordx, int cordy,struct Bullet*);

int areCornersClear(struct Tank tanks[MAX_BOTS + 1], int *flag_left, int*flag_right);

int canGoRight(int x, int y, int index, int bullet_or_tank /* 0-bullet, 1-tank*/, struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int** map);

void make_the_move(int next_moves[MAX_BOTS + 1], struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int **map,int);

void moveTheBullets(struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], struct PowerUp* power, int **map, int *done, int cordx, int cordy, int *lives,int,int);

void createEnemie(struct Tank tanks[MAX_BOTS + 1], int ,int);

int* nextMoves(struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int** map);

int canGoUp(int x, int y, int index, int bullet_or_tank /* 0-bullet, 1-tank*/, struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int** map);

int canGoDown(int x, int y, int index, int bullet_or_tank /* 0-bullet, 1-tank*/, struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int** map);

int canGoLeft(int x, int y, int index, int bullet_or_tank /* 0-bullet, 1-tank*/, struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int** map);

int canGoRight(int x, int y, int index, int bullet_or_tank /* 0-bullet, 1-tank*/, struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int** map);

void paintLevel(int **map, SDL_Texture* map_elements[TOTAL_ELEMENTS],int,int);

void createMaps(SDL_Texture* map_elements[TOTAL_ELEMENTS]);

int botsAlive(struct Tank tanks[MAX_BOTS + 1]);

int isThereBullet(int direction, int index, struct Bullet bullets[MAX_BOTS + 1]);

void cleanSpots(int ***a);

void delPower(struct PowerUp* power);

void makePower(struct PowerUp* power);

void takePowerup(int next_moves[MAX_BOTS + 1], struct Tank tanks[MAX_BOTS + 1], struct PowerUp*power, int*lives, int***maps, int lvl,int*,int*);

int hitEagle(struct Bullet bullets[MAX_BOTS + 1]);

void updateWall(int cime, int**map);

void writeText(const char *text, TTF_Font *g_font, int x, int y, int height, int width, SDL_Color front_color, SDL_Color back_color);

void doteraj_mape(int ***a, int lvl);

void upisi(int ***a);

SCORES *starting_scores();

char* newHighScore();

int isItEagle(int**map, int i, int j);

int bfs(int **map, int tankx, int tanky, int task, int index, struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int **matrica);

int dodge(struct Tank tenkic, struct Bullet metak, int **map, int move);

int easy(struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int** map, int moduo, int i);

int* premedium(struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int** matrica);

int medium(struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int** matrica, int i);

int hard(struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int** matrica, int i);

#endif
