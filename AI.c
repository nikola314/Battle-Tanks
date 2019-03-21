
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include "functions.h"
#define MOD 10000

/*
pretpostavio sam postojanje funkcija koje vracaju boolove
uz zavisnosti od toga da li je nesto levo ispred ili iza tenkica
za smer sam kositio 0 gore, 1 desno, 2 dole, 3 levo radi jednostavnosti promene smera
pretpostavljam zasad i da tenkic moze da se okrene za 180 stepeni
nakon funkcija matcopy koristimo oznake
0 - prazno
1 - obican zid
2 - celicni zid
3 - voda
9 - bot
8 - igrac
7 - zid oko ptice
6 - ptica

kod lakog se koristimmo trenutkom poslednjeg okreta i u zavisnoti od toga odredjujemo sansu za naredni okret

kod srednjeg se koristimo trenutkom poslednje promene odluke i na osnovu toga odredjujemo sansu za sledecu promenu odluke

*/

struct point
{
	int x, y;
};

int last_turn[100] = { 0 }, moduo = 0, task[100];
int n, m;   // dimenzije matrice


//pravimo novu matricu u koju ubacujemo i tenkice igraca kao i botova. ubacujemo i bazu kao i zidove oko baze
int **matCopy(int** matrica, struct Tank tanks[MAX_BOTS + 1]) {
	int i, j, k,**map;
	map = (int**)malloc(n * sizeof(int*));
	for (i = 0; i < n; i++)
	{
		map[i] = (int*)malloc(m * sizeof(int));
	}
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < m; j++)
		{
			map[i][j] = matrica[i][j];
		}
	}
	for (j = tanks[0].y / UNIT; j<tanks[0].y / UNIT + 4; j++)//ako je sirina tenka 4
	{
		for (k = tanks[0].x / UNIT; k <= tanks[0].x / UNIT + 4; k++) {
			map[j][k] = 8; //ako je 8 neki fleg da ti je tu tenk igrac
		}
	}

	for (i = 1; i<MAX_BOTS + 1; i++) {
		for (j = tanks[i].y / UNIT; j<tanks[i].y / UNIT + 4; j++)//ako je sirina tenka 4
		{
			if (tanks[i].x >= 0) {
				for (k = tanks[i].x / UNIT; k <= tanks[i].x / UNIT + 4; k++) {
					map[j][k] = 9; //ako je 9 neki fleg da ti je tu tenk bot
				}
			}
		}
	}
	for (i = SCREEN_HEIGHT / UNIT - 6; i<SCREEN_HEIGHT / UNIT; i++)    // zidovi
	{
		for (j = SCREEN_WIDTH / UNIT / 2 - 4; j <= SCREEN_WIDTH / UNIT / 2 + 3; j++)
		{
			if (matrica[i][j] == 1)
				map[i][j] = 7;
		}
	}

	for (i = SCREEN_HEIGHT / UNIT - 4; i<SCREEN_HEIGHT / UNIT; i++)    // bazu
	{
		for (j = SCREEN_WIDTH / UNIT / 2 - 2; j <= SCREEN_WIDTH / UNIT / 2 + 1; j++)
		{
			map[i][j] = 6;
		}
	}

	return map;
}


//blok od naredne cetiri funkcije proverava da li se nesto tipa zid oko ptice ili igrac nalazi u nekom od pravaca
int gore(struct Tank tenkic, int **map, int target)
{
	int i, j, q;
	for (i = tenkic.y / UNIT; i >= 0; i--)
	{
		q = 0;
		for (j = tenkic.x / UNIT + 1; j<tenkic.x / UNIT + 3; j++)
		{
			if ((map[i][j] == 3))
			{
				return 0;
			}
			if (map[i][j] == target)
			{
				return 1;
			}
		}
		if (q == 4)  // postoji neki zid u nekom pravcu
			return 0;
	}
	return 0;
}

int desno(struct Tank tenkic, int **map, int target)
{
	int i, j, q;
	for (i = tenkic.y / UNIT + 1; i<tenkic.y / UNIT + 3; i++)
	{
		q = 0;
		for (j = tenkic.x / UNIT; j<m; j++)
		{
			if ((map[i][j] == 3))
			{
				return 0;
			}
			if (map[i][j] == target)
			{
				return 1;
			}
		}
		if (q == 4)
		{
			return 0;
		}
	}
	return 0;
}

int dole(struct Tank tenkic, int **map, int target)
{
	int i, j, q;
	for (i = tenkic.y / UNIT; i<n; i++)
	{
		q = 0;
		for (j = tenkic.x / UNIT + 1; j<tenkic.x / UNIT + 3; j++)
		{
			if ((map[i][j] == 3))
			{
				return 0;
			}
			if (map[i][j] == target)
			{
				return 1;
			}
		}
		if (q == 4)
		{
			return 0;
		}
	}
	return 0;
}

int levo(struct Tank tenkic, int **map, int target)
{
	int i, j, q;
	for (j = tenkic.x / UNIT; j >= 0; j--)
	{
		q = 0;
		for (i = tenkic.y / UNIT + 1; i<tenkic.y / UNIT + 3; i++)
		{
			if ((map[i][j] == 3))
			{
				return 0;
			}
			if (map[i][j] == target)
			{
				return 1;
			}
		}
		if (q == 4)
		{
			return 0;
		}
	}
	return 0;
}

// proveravamo da li je ispred tenkica igrac
int aheadTank(struct Tank tenkic, int **map)
{
	if (tenkic.direction == 0)
	{
		return gore(tenkic, map, 8);
	}
	if (tenkic.direction == 1)
	{
		return desno(tenkic, map, 8);
	}
	if (tenkic.direction == 2)
	{
		return dole(tenkic, map, 8);
	}
	if (tenkic.direction == 3)
	{
		return levo(tenkic, map, 8);
	}
}

// proveravamo da li je ispred tenkica baza
int aheadPtica(struct Tank tenkic, int **map)
{
	if (tenkic.direction == 0)
	{
		return (gore(tenkic, map, 7) + gore(tenkic, map, 6));
	}
	if (tenkic.direction == 1)
	{
		return (desno(tenkic, map, 7) + desno(tenkic, map, 6));
	}
	if (tenkic.direction == 2)
	{
		return (dole(tenkic, map, 7) + dole(tenkic, map, 6));
	}
	if (tenkic.direction == 3)
	{
		return (levo(tenkic, map, 7) + levo(tenkic, map, 6));
	}
}

// odredjuje najkraci put do igaca ili ptice, koristi se bfs algoritmom pretrage najkraceg puta




int bfs(int **map, int tankx, int tanky, int task, int index, struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int **matrica)
{
	int br, qx, qy, bio[100][100], bx[10000], by[10000], last_move = 0, i, j, ii, rr, prev_move = 0;

	br = -1;
	for (i = 0; i<n; i++)  // nigde nisam bio
	{
		for (j = 0; j<m; j++)
		{
			bio[i][j] = 5;
		}
	}
	for (i = tankx; i <= tankx + 3; i++)  // potrpamo ivice tenkica u red za bfs
	{
		br++;
		bx[br] = i;
		by[br] = tanky;
		bio[tanky][i] = 1;
		br++;
		bx[br] = i;
		by[br] = tanky + 3;
		bio[tanky + 3][i] = 1;
	}
	for (i = tanky + 1; i <= tanky + 2; i++)  // potrpamo ivice tenkica u red za bfs
	{
		br++;
		bx[br] = tankx;
		by[br] = i;
		bio[i][tankx] = 1;
		br++;
		bx[br] = tankx + 3;
		by[br] = i;
		bio[i][tankx + 3] = 1;
	}
	for (i = 0; i <= br; i++)
	{
		qx = bx[i];
		qy = by[i];
		if (map[qy][qx] == task)  //rekonstrukcija
		{
			while (1)
			{
				if (map[qy][qx] == 9)
				{
					if (last_move == 0)
					{
						if (canGoUp(tankx*UNIT, tanky*UNIT, index, 1, tanks, bullets, matrica) != IS_EMPTY)
						{
							rr = rand() % 2;
							if (rr == 0)
							{
								return 1;
							}
							else
							{
								return 3;
							}
						}
					}
					if (last_move == 2)
					{
						if (canGoDown(tankx*UNIT, tanky*UNIT, index, 1, tanks, bullets, matrica) != IS_EMPTY)
						{
							rr = rand() % 2;
							if (rr == 0)
							{
								return 1;
							}
							else
							{
								return 3;
							}
						}
					}
					if (last_move == 1)
					{
						if (canGoRight(tankx*UNIT, tanky*UNIT, index, 1, tanks, bullets, matrica) != IS_EMPTY)
						{
							rr = rand() % 2;
							if (rr == 0)
							{
								return 0;
							}
							else
							{
								return 2;
							}
						}
					}
					if (last_move == 3)
					{
						if (canGoLeft(tankx*UNIT, tanky*UNIT, index, 1, tanks, bullets, matrica) != IS_EMPTY)
						{
							rr = rand() % 2;
							if (rr == 0)
							{
								return 0;
							}
							else
							{
								return 2;
							}
						}
					}
					return last_move;
				}
				if (bio[qy][qx] == 0)
				{
					qy++;
					last_move = 0;
				}
				if (bio[qy][qx] == 1)
				{
					qx--;
					last_move = 1;
				}
				if (bio[qy][qx] == 2)
				{
					qy--;
					last_move = 2;
				}
				if (bio[qy][qx] == 3)
				{
					qx++;
					last_move = 3;
				}
			}
		}
		if (qx - 1 >= 0)
		{
			if ((bio[qy][qx - 1] == 5) && ((map[qy][qx - 1] == 0) || (map[qy][qx - 1] == task)))
			{
				br++;
				bio[qy][qx - 1] = 3;
				bx[br] = qx - 1;
				by[br] = qy;
			}
		}
		if (qy - 1 >= 0)
		{
			if ((bio[qy - 1][qx] == 5) && ((map[qy - 1][qx] == 0) || (map[qy - 1][qx] == task)))
			{
				br++;
				bio[qy - 1][qx] = 0;
				bx[br] = qx;
				by[br] = qy - 1;
			}
		}
		if (qx + 1<m)
		{
			if ((bio[qy][qx + 1] == 5) && ((map[qy][qx + 1] == 0) || (map[qy][qx + 1] == task)))
			{
				br++;
				bio[qy][qx + 1] = 1;
				bx[br] = qx + 1;
				by[br] = qy;
			}
		}
		if (qy + 1<n)
		{
			if ((bio[qy + 1][qx] == 5) && ((map[qy + 1][qx] == 0) || (map[qy + 1][qx] == task)))
			{
				br++;
				bio[qy + 1][qx] = 2;
				bx[br] = qx;
				by[br] = qy + 1;
			}
		}
	}
	//ako nema gde vrati bilo gde
	return easy(tanks, bullets, map, moduo, index);
}

int aheadZid(struct Tank tenkic, int **map)
{
	int i, j, dx, dy, q;
	if (tenkic.direction == 0)
	{
		dy = tenkic.y / UNIT - 1;
		dx = tenkic.x / UNIT + 1;
		for (i = dy; i > dy - 4; i--)
		{
			q = 0;
			for (j = dx; j < dx + 2; j++)
			{
				if (i < 0)
					return 0;
				if (map[i][j] == 1)
					return 1;
				if (map[i][j] == STEEL)
					q++;
				if (map[i][j] == 9)
					return 0;
			}
			if (q == 2)
				return 0;
		}
		return 0;
	}
	if (tenkic.direction == 2)
	{
		dy = tenkic.y / UNIT + 4;
		dx = tenkic.x / UNIT + 1;
		for (i = dy; i < dy + 4; i++)
		{
			q = 0;
			for (j = dx; j < dx + 2; j++)
			{
				if (i >= n)
					return 0;
				if (map[i][j] == 1)
					return 1;
				if (map[i][j] == STEEL)
					q++;
				if (map[i][j] == 9)
					return 0;
			}
			if (q == 2)
				return 0;
		}
		return 0;
	}
	if (tenkic.direction == 3)
	{
		dy = tenkic.y / UNIT + 1;
		dx = tenkic.x / UNIT - 1;
		for (j = dx; j>dx - 4; j--)
		{
			q = 0;
			for (i = dy; i <= dy + 1; i++)
			{
				if (j < 0)
					return 0;
				if (map[i][j] == 1)
					return 1;
				if (map[i][j] == STEEL)
					q++;
				if (map[i][j] == 9)
					return 0;
			}
			if (q == 2)
				return 0;
		}
		return 0;
	}
	if (tenkic.direction == 1)
	{
		dy = tenkic.y / UNIT + 1;
		dx = tenkic.x / UNIT + 4;
		for (j = dx; j<dx + 4; j++)
		{
			q = 0;
			for (i = dy; i <= dy + 1; i++)
			{
				if (j >= m)
					return 0;
				if (map[i][j] == 1)
					return 1;
				if (map[i][j] == STEEL)
					q++;
				if (map[i][j] == 9)
					return 0;
			}
			if (q == 2)
				return 0;
		}
		return 0;
	}
}

//funkcija koja pazi da ne naletimo na metak huehuehueue
int dodge(struct Tank tenkic, struct Bullet metak, int **map, int move)
{
	int i, j;
	if (metak.x < 0) return -1;
	if (metak.direction == 0)
	{
		for (i = metak.y / UNIT - 1; i >= 0; i--)
		{
			for (j = metak.x / UNIT; j <= metak.x / UNIT + 1; j++)
			{
				if ((map[i][j] == 1) || (map[i][j] == 2))  // metak udara u zid
				{
					return -1;
				}
				if ((move == 1) && (i == tenkic.y / UNIT) && (j == tenkic.x / UNIT + 4))
				{
					return 3;
				}
				if ((move == 3) && (i == tenkic.y / UNIT) && (j == tenkic.x / UNIT - 2))
				{
					return 1;
				}
			}
		}
		return -1;
	}
	if (metak.direction == 2)
	{
		for (i = metak.y / UNIT + 1; i<n; i++)
		{
			for (j = metak.x / UNIT; j <= metak.x / UNIT + 1; j++)
			{
				if ((map[i][j] == 1) || (map[i][j] == 2))  // metak udara u zid
				{
					return -1;
				}
				if ((move == 1) && (i == tenkic.y / UNIT) && (j == tenkic.x / UNIT + 4))
				{
					return 3;
				}
				if ((move == 3) && (i == tenkic.y / UNIT) && (j == tenkic.x / UNIT - 2))
				{
					return 1;
				}
			}
		}
		return -1;
	}
	if (metak.direction == 1)
	{
		for (j = metak.x / UNIT + 1; j<m; j++)
		{
			for (i = metak.y / UNIT; i<metak.y / UNIT + 2; i++)
			{
				if ((map[i][j] == 1) || (map[i][j] == 2))  // metak udara u zid
				{
					return -1;
				}
				if ((move == 0) && (j == tenkic.x / UNIT) && (i == tenkic.y / UNIT - 2))
				{
					return 2;
				}
				if ((move == 2) && (j == tenkic.x / UNIT) && (i == tenkic.y / UNIT + 4))
				{
					return 0;
				}
			}
		}
		return -1;
	}
	if (metak.direction == 3)
	{
		for (j = metak.x / UNIT - 1; j >= 0; j--)
		{
			for (i = metak.y / UNIT; i<metak.y / UNIT + 2; i++)
			{
				if ((map[i][j] == 1) || (map[i][j] == 2))  // metak udara u zid
				{
					return -1;
				}
				if ((move == 0) && (j == tenkic.x / UNIT) && (i == tenkic.y / UNIT - 2))
				{
					return 2;
				}
				if ((move == 2) && (j == tenkic.x / UNIT) && (i == tenkic.y / UNIT + 4))
				{
					return 0;
				}
			}
		}
		return -1;
	}
}

int easy(struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int** map, int moduo, int i)
{
	int  smer[10], broj_smer = 0, gde_ici, nastavlja, keep_goin;
	srand(time(NULL));
	int next_moves;
	if (tanks[i].x < 0)
		return -1;

	broj_smer = -1;
	if (canGoUp(tanks[i].x, tanks[i].y, i, 1, tanks, bullets, map) == IS_EMPTY)
	{
		broj_smer++;
		smer[broj_smer] = 0;
	}
	if (canGoLeft(tanks[i].x, tanks[i].y, i, 1, tanks, bullets, map) == IS_EMPTY)
	{
		broj_smer++;
		smer[broj_smer] = 3;
	}
	if (canGoDown(tanks[i].x, tanks[i].y, i, 1, tanks, bullets, map) == IS_EMPTY)
	{
		broj_smer++;
		smer[broj_smer] = 2;
	}
	if (canGoRight(tanks[i].x, tanks[i].y, i, 1, tanks, bullets, map) == IS_EMPTY)
	{
		broj_smer++;
		smer[broj_smer] = 1;
	}
	if (bullets[i].x <0)  // ukoliko moze da puca stavljamo dva ta u niz da bi sanse da puca bile malo vece
	{
		broj_smer++;
		smer[broj_smer] = SHOOT;
	}
	keep_goin = 100 - (moduo - last_turn[i]) % MOD;   // sansa da tenkic nastavi da se krece u smeru u kom je isao
													  // sansa raste sto se vise odaljavamo od momenta poslednjeg okretanja
	nastavlja = rand() % 100;   // slucajno biramo da li tenkic skrece ili 
	if ((tanks[i].direction == 0) && (canGoUp(tanks[i].x, tanks[i].y, i, 1, tanks, bullets, map) != IS_EMPTY)) // sanse se smanjuju tri puta da ide pravo ako je nesto ispred
	{
		keep_goin = 0;
	}
	if ((tanks[i].direction == 1) && (canGoRight(tanks[i].x, tanks[i].y, i, 1, tanks, bullets, map) != IS_EMPTY)) // sanse se smanjuju tri puta da ide pravo ako je nesto ispred
	{
		keep_goin = 0;
	}
	if ((tanks[i].direction == 2) && (canGoDown(tanks[i].x, tanks[i].y, i, 1, tanks, bullets, map) != IS_EMPTY)) // sanse se smanjuju tri puta da ide pravo ako je nesto ispred
	{
		keep_goin = 0;
	}
	if ((tanks[i].direction == 3) && (canGoLeft(tanks[i].x, tanks[i].y, i, 1, tanks, bullets, map) != IS_EMPTY)) // sanse se smanjuju tri puta da ide pravo ako je nesto ispred
	{
		keep_goin = 0;
	}
	if (nastavlja<keep_goin)// nastavlja kud je isao
	{
		next_moves = tanks[i].direction;
	}
	else   // zaokrece
	{
		if (broj_smer >= 0)
		{
			gde_ici = rand() % (broj_smer + 1);
			last_turn[i] = moduo;
			if (smer[gde_ici] < 4)  // kretanje je u pitanju
				next_moves = smer[gde_ici];
			else // pucanje je u pitanju
				next_moves = smer[gde_ici];
		}
		else
		{
			next_moves = SHOOT;
		}
	}
	return next_moves;
}

// kao easy samo kad te vidi da puca
int* premedium(struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int** matrica)
{
	int broj_smer, smer[6], i, rr, keep_goin,**map;
	int *next_moves;
	next_moves = malloc((MAX_BOTS + 1) * sizeof(int));

	map=matCopy(matrica, tanks);

	for (i = 1; i < MAX_BOTS + 1; i++)
	{
		if (tanks[i].x < 0)
			continue;

		broj_smer = -1;
		if (canGoUp(tanks[i].x, tanks[i].y, i, 1, tanks, bullets, matrica) == IS_EMPTY)
		{
			broj_smer++;
			smer[broj_smer] = 0;
		}
		if (canGoLeft(tanks[i].x, tanks[i].y, i, 1, tanks, bullets, matrica) == IS_EMPTY)
		{
			broj_smer++;
			smer[broj_smer] = 3;
		}
		if (canGoDown(tanks[i].x, tanks[i].y, i, 1, tanks, bullets, matrica) == IS_EMPTY)
		{
			broj_smer++;
			smer[broj_smer] = 2;
		}
		if (canGoRight(tanks[i].x, tanks[i].y, i, 1, tanks, bullets, matrica) == IS_EMPTY)
		{
			broj_smer++;
			smer[broj_smer] = 1;
		}
		if (bullets[i].x < 0)
		{
			broj_smer++;
			smer[broj_smer] = SHOOT;
			broj_smer++;
			smer[broj_smer] = SHOOT;
		}
		if (aheadTank(tanks[i], map) && bullets[i].x < 0)
		{
			next_moves[i] = SHOOT;
			continue;
		}
		if (gore(tanks[i], map, 8))  // menjamo zadatak tenkica ako vidi igraca
		{
			next_moves[i] = 0;
			continue;
		}
		if (dole(tanks[i], map, 8))
		{
			next_moves[i] = 2;
			continue;
		}
		if (levo(tanks[i], map, 8))
		{
			next_moves[i] = 3;
			continue;
		}
		if (desno(tanks[i], map, 8))
		{
			next_moves[i] = 1;
			continue;
		}
		keep_goin = 100 - (moduo - last_turn[i]) % MOD;
		if ((tanks[i].direction == 0) && (canGoUp(tanks[i].x, tanks[i].y, i, 1, tanks, bullets, matrica) != IS_EMPTY)) // sanse se smanjuju tri puta da ide pravo ako je nesto ispred
		{
			keep_goin = 0;
		}
		if ((tanks[i].direction == 1) && (canGoRight(tanks[i].x, tanks[i].y, i, 1, tanks, bullets, matrica) != IS_EMPTY)) // sanse se smanjuju tri puta da ide pravo ako je nesto ispred
		{
			keep_goin = 0;
		}
		if ((tanks[i].direction == 2) && (canGoDown(tanks[i].x, tanks[i].y, i, 1, tanks, bullets, matrica) != IS_EMPTY)) // sanse se smanjuju tri puta da ide pravo ako je nesto ispred
		{
			keep_goin = 0;
		}
		if ((tanks[i].direction == 3) && (canGoLeft(tanks[i].x, tanks[i].y, i, 1, tanks, bullets, matrica) != IS_EMPTY)) // sanse se smanjuju tri puta da ide pravo ako je nesto ispred
		{
			keep_goin = 0;
		}
		rr = rand() % 100;
		if (rr <= keep_goin)
		{
			next_moves[i] = tanks[i].direction;
			continue;
		}
		if (broj_smer > 0)
		{
			rr = rand() % broj_smer;
			last_turn[i] = moduo;
			next_moves[i] = smer[rr];
		}
		else
			next_moves[i] = SHOOT;

	}
	return next_moves;
}

int medium(struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int** matrica, int i)
{
	int  next_moves, sta_radi, balans_sansa, j, izbegavanje,**map;


	if (tanks[i].x < 0)
		return -1;
	map=matCopy(matrica, tanks);
	sta_radi = rand() % 100;
	if (sta_radi < 20)
	{
		next_moves = tanks[i].direction;
	}
	balans_sansa = (moduo - last_turn[i]) % MOD; // odredjivanje sa kojom ce sansom tenkic da juri coveka, a sa kojom pticu
	balans_sansa = min(balans_sansa, 60);    // ostaje uvek pozitivna
	if ((sta_radi >= 20) && (sta_radi<30 + balans_sansa))   // menja odluku
	{
		task[i] = (task[i] - 7 + 1) % 2 + 7;
		last_turn[i] = moduo;
		next_moves = bfs(map, tanks[i].x / UNIT, tanks[i].y / UNIT, task[i], i, tanks, bullets, matrica);   //  u oba slucaja pozivamo bfs funkciju za odredjivanje sledeceg poteza samo sa razlicitom metom
	}
	if ((sta_radi >= 30 + balans_sansa) && (sta_radi<100))
	{
		next_moves = bfs(map, tanks[i].x / UNIT, tanks[i].y / UNIT, task[i], i, tanks, bullets, matrica);
	}
	if (aheadTank(tanks[i], map) || aheadPtica(tanks[i], map))
	{
		if (bullets[i].x < 0)
		{
			balans_sansa = 80;  // sansa da zapuca ukoliko je igrac ispred njega je 80%
			if (rand() % 100 < balans_sansa)
			{
				next_moves = SHOOT;
			}
		}
	}
	else    // sansa da po randomu zapuca je 20%
	{
		if (bullets[i].x < 0)
		{
			balans_sansa = 20;
			if (rand() % 100 < balans_sansa)
			{
				next_moves = SHOOT;
			}
		}
	}
	izbegavanje = dodge(tanks[i], bullets[0], map, next_moves);
	if (izbegavanje>-1)
	{
		next_moves = izbegavanje;
	}
	return next_moves;
}

int hard(struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int** matrica, int i)
{
	int  izbegavanje,**map;
	int next_moves;

	if (tanks[i].x < 0)
		return -1;
	map=matCopy(matrica, tanks);
	if (bullets[i].x < 0)
	{
		if ((aheadTank(tanks[i], map)) || (aheadPtica(tanks[i], map)))  // ako su tenkic ili ptica ispred njega onda ce da puca
		{
			next_moves = SHOOT;
			return next_moves;
		}
	}
	if (gore(tanks[i], map, 8))  // menjamo zadatak tenkica ako vidi igraca
	{
		next_moves = 0;
		task[i] = 8;
		return next_moves;
	}
	if (dole(tanks[i], map, 8))
	{
		next_moves = 2;
		task[i] = 8;
		return next_moves;
	}
	if (levo(tanks[i], map, 8))
	{
		next_moves = 3;
		task[i] = 8;
		return next_moves;
	}
	if (desno(tanks[i], map, 8))
	{
		next_moves = 1;
		task[i] = 8;
		return next_moves;
	}
	if ((dole(tanks[i], map, 7)) || (dole(tanks[i], map, 6)))   // menjamo zadatak tenkica ako je ispred njega ptica
	{
		next_moves = 2;
		task[i] = 7;
		return next_moves;
	}
	if ((levo(tanks[i], map, 7)) || (levo(tanks[i], map, 6)))
	{
		next_moves = 3;
		task[i] = 7;
		return next_moves;
	}
	if ((desno(tanks[i], map, 7)) || (desno(tanks[i], map, 6)))
	{
		next_moves = 1;
		task[i] = 7;
		return next_moves;
	}



	if (task[i] == 8)    // ispunjava svoj zadatak
	{
		next_moves = bfs(map, tanks[i].x / UNIT, tanks[i].y / UNIT, 8, i, tanks, bullets, matrica);
	}
	else
	{
		next_moves = bfs(map, tanks[i].x / UNIT, tanks[i].y / UNIT, 7, i, tanks, bullets, matrica);
	}

	//da puca u zidove od cigle ako im je blizu
	if ((aheadZid(tanks[i], map)) && (bullets[i].x<0))
	{
		next_moves = SHOOT;
	}

	izbegavanje = dodge(tanks[i], bullets[0], map, next_moves);
	if (izbegavanje>-1)
	{
		next_moves = izbegavanje;
	}
	last_turn[i] = next_moves;
	return next_moves;
}

int* nextMoves(struct Tank tanks[MAX_BOTS + 1], struct Bullet bullets[MAX_BOTS + 1], int** map)
{
	int i, *potez;
	potez = (int*)malloc(sizeof(int)*(MAX_BOTS + 1));
	int tezina = DIFFICULTY;
	n = SCREEN_HEIGHT / UNIT;   // cisto podesavamo ovako jer sam ja tako koristio u kodu :)
	m = SCREEN_WIDTH / UNIT;
	moduo += 3;   // uvecavamo broj poziva funkcije
	moduo = moduo%MOD;
	if (tezina == 1)
	{
		for (i = 1; i <= MAX_BOTS; i++)
			potez[i] = easy(tanks, bullets, map, moduo, i);
		return potez;
	}
	if (tezina == 2)
	{
		return premedium(tanks, bullets, map, last_turn);
	}
	if (tezina == 3)
	{
		if (moduo == 3)
		{
			for (i = 1; i <= MAX_BOTS; i++)
			{
				task[i] = 8;
				last_turn[i] = 0;
			}
		}
		for (i = 1; i <= MAX_BOTS; i++)
			potez[i] = medium(tanks, bullets, map, i);
		return potez;
	}
	if (tezina == 4)
	{
		for (i = 1; i <= MAX_BOTS; i++)   // na hard ce botovi da imaju fino odredjene zadatke trecina na pticu dve trecine na igraca
		{
			if (moduo == 3)
			{
				if (i % 3 == 1)
				{
					task[i] = 7;
				}
				else
				{
					task[i] = 8;
				}
			}
			potez[i] = hard(tanks, bullets, map, i);
		}
		return potez;
	}
}