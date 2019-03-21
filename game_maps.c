#include <stdio.h>
#include <SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include <string.h>
#include "functions.h"
#include<stdlib.h>
#include<time.h>



int created = 0;

//Oslobadja mesta gde treba da se rode tenkovi i orao - na svim mapama
void cleanSpots(int ***a){
	int i, j, k;
	for (i = 1; i <= MAX_LEVELS; i++)
	{

		//mesta za botove
		for (j = 0; j < TANK_WIDTH; j++)
			for (k = 0; k < TANK_WIDTH; k++)
				a[i][j][k] = BLACK;

		for (j = 0; j < TANK_WIDTH; j++)
			for (k = ((SCREEN_WIDTH / UNIT) - TANK_WIDTH); k < SCREEN_WIDTH / UNIT; k++)
				a[i][j][k] = BLACK;
		//zastitni sloj oko orla
		for (j = (SCREEN_HEIGHT / UNIT - 3 * TANK_WIDTH / 2); j < SCREEN_HEIGHT / UNIT - TANK_WIDTH; j++)
			for (k = SCREEN_WIDTH / (2 * UNIT) - TANK_WIDTH; k < SCREEN_WIDTH / (2 * UNIT) + TANK_WIDTH; k++)
				a[i][j][k] = BRICK;

		for (j = (SCREEN_HEIGHT / UNIT - TANK_WIDTH); j < SCREEN_HEIGHT / UNIT; j++)
			for (k = SCREEN_WIDTH / (2 * UNIT) - TANK_WIDTH; k < SCREEN_WIDTH / (2 * UNIT) - TANK_WIDTH / 2; k++)
				a[i][j][k] = BRICK;

		for (j = (SCREEN_HEIGHT / UNIT - TANK_WIDTH); j < SCREEN_HEIGHT / UNIT; j++)
			for (k = SCREEN_WIDTH / (2 * UNIT) + TANK_WIDTH / 2; k < SCREEN_WIDTH / (2 * UNIT) + TANK_WIDTH; k++)
				a[i][j][k] = BRICK;

		//orao
		for (j = (SCREEN_HEIGHT / UNIT - TANK_WIDTH); j < SCREEN_HEIGHT / UNIT; j++)
			for (k = SCREEN_WIDTH / (2 * UNIT) - TANK_WIDTH / 2; k < SCREEN_WIDTH / (2 * UNIT) + TANK_WIDTH / 2; k++)
				a[i][j][k] = BLACK;
		// player
		for (j = (SCREEN_HEIGHT / UNIT - TANK_WIDTH); j < SCREEN_HEIGHT / UNIT; j++)
			for (k = SCREEN_WIDTH / (2 * UNIT) - 2 * TANK_WIDTH; k < SCREEN_WIDTH / (2 * UNIT) - TANK_WIDTH; k++)
				a[i][j][k] = BLACK;

	}
}


void INSERT_O(int **mat, int m, int n, int x)
{
	int i, j;
	for (i = m; i < m + 4; i++)
		for (j = n; j < n + 4; j++)
			mat[i][j] = x;
}

void INSERT_OO(int **mat, int m, int n, int x)
{
	int i, j;
	for (i = m; i < m +2; i++)
		for (j = n; j < n +2; j++)
			mat[i][j] = x;
}

void random_mapa(int ***a, int lvl)
{
	int i, j, k;
	int m = SCREEN_HEIGHT, n = SCREEN_WIDTH;
	int r;
	m = m / UNIT;
	n = n / UNIT;

	if(lvl==1) srand(time(NULL));

	for (i = lvl; i <= lvl; i++)
		for (j = 0; j < m; j += 4)
			for (k = 0; k < n; k += 4)
				if ((r = rand()) % 4) a[i][j][k] = BLACK;
				else
				{
					r /= 4;
					if (r % 4) 
					{
						if (r % 3) INSERT_O(a[i], j, k, BRICK);
						else
						{
							r /= 3;
							if (r % 3) INSERT_O(a[i], j, k, WATER);
							else INSERT_O(a[i], j, k, STEEL);
						}
					}
					/*else
					{
						if (r % 3) INSERT_OO(a[i], j, k, BRICK);
						else 
						{
							r /= 3;
							if (r % 3) INSERT_OO(a[i], j, k, WATER);
							else INSERT_OO(a[i], j, k, STEEL);
						}
					}*/
				}
	doteraj_mape(a, lvl);
				return ;
}
				
int *** create_map()
{
	int ***a, i, j, k;
	int m = SCREEN_HEIGHT, n = SCREEN_WIDTH;
	m = m / UNIT;
	n = n / UNIT;

	a = malloc(100 * sizeof(int**));
	a[0] = NULL;
	for (k = 1; k<100; k++)
	{
		a[k] = malloc(m * sizeof(int*));
		for (i = 0; i<m; i++)
		{
			a[k][i] = malloc(n * sizeof(int));
			for (j = 0; j<n; j++)
				a[k][i][j] = BLACK;
		}
	}

	return a;
}

void doteraj_mape(int ***a, int lvl)
{
	int i, j, k;

	for (i = lvl; i <= lvl; i++)
	{
		//mesta za botove
		for (j = 0; j<TANK_WIDTH; j++)
			for (k = 0; k<TANK_WIDTH; k++)
				a[i][j][k] = BLACK;

		for (j = 0; j<TANK_WIDTH; j++)
			for (k = ((SCREEN_WIDTH / UNIT) - TANK_WIDTH); k<SCREEN_WIDTH / UNIT; k++)
				a[i][j][k] = BLACK;
		//zastitni sloj oko orla
		for (j = (SCREEN_HEIGHT / UNIT - 3 * TANK_WIDTH / 2); j<SCREEN_HEIGHT / UNIT - TANK_WIDTH; j++)
			for (k = SCREEN_WIDTH / (2 * UNIT) - TANK_WIDTH; k<SCREEN_WIDTH / (2 * UNIT) + TANK_WIDTH; k++)
				a[i][j][k] = BRICK;

		for (j = (SCREEN_HEIGHT / UNIT - TANK_WIDTH); j<SCREEN_HEIGHT / UNIT; j++)
			for (k = SCREEN_WIDTH / (2 * UNIT) - TANK_WIDTH; k<SCREEN_WIDTH / (2 * UNIT) - TANK_WIDTH / 2; k++)
				a[i][j][k] = BRICK;

		for (j = (SCREEN_HEIGHT / UNIT - TANK_WIDTH); j<SCREEN_HEIGHT / UNIT; j++)
			for (k = SCREEN_WIDTH / (2 * UNIT) + TANK_WIDTH / 2; k<SCREEN_WIDTH / (2 * UNIT) + TANK_WIDTH; k++)
				a[i][j][k] = BRICK;

		//orao
		for (j = (SCREEN_HEIGHT / UNIT - TANK_WIDTH); j<SCREEN_HEIGHT / UNIT; j++)
			for (k = SCREEN_WIDTH / (2 * UNIT) - TANK_WIDTH / 2; k<SCREEN_WIDTH / (2 * UNIT) + TANK_WIDTH / 2; k++)
				a[i][j][k] = BLACK;
		// player
		for (j = (SCREEN_HEIGHT / UNIT - TANK_WIDTH); j<SCREEN_HEIGHT / UNIT; j++)
			for (k = SCREEN_WIDTH / (2 * UNIT) - 2 * TANK_WIDTH; k<SCREEN_WIDTH / (2 * UNIT) - TANK_WIDTH; k++)
				a[i][j][k] = BLACK;
	}


	// Levi bot
	INSERT_O(a[lvl], 4, 0, BLACK);
	INSERT_O(a[lvl], 4, 4, BLACK);
	INSERT_O(a[lvl], 0, 4, BLACK);
	// Player
	INSERT_O(a[lvl], SCREEN_HEIGHT / UNIT - 2 * TANK_WIDTH, SCREEN_WIDTH / (2 * UNIT) - 2 * TANK_WIDTH, BLACK);
	INSERT_O(a[lvl], SCREEN_HEIGHT / UNIT - 2 * TANK_WIDTH, SCREEN_WIDTH / (2 * UNIT) - 3 * TANK_WIDTH, BLACK);
	INSERT_O(a[lvl], SCREEN_HEIGHT / UNIT - TANK_WIDTH, SCREEN_WIDTH / (2 * UNIT) - 3 * TANK_WIDTH, BLACK);
	//Desni bot
	INSERT_O(a[lvl], (SCREEN_WIDTH / UNIT) - 2 * TANK_WIDTH, 0, BLACK);
	INSERT_O(a[lvl], (SCREEN_WIDTH / UNIT) - 2 * TANK_WIDTH, 4, BLACK);
	INSERT_O(a[lvl], (SCREEN_WIDTH / UNIT) - TANK_WIDTH, 4, BLACK);

}
/*
void random_mapa(int ***a, int lvl)
{
	int i, j, k;
	int m = SCREEN_HEIGHT, n = SCREEN_WIDTH;
	int r;
	m = m / UNIT;
	n = n / UNIT;

	for (i = lvl; i <= lvl; i++)
		for (j = 0; j<m; j++)
			for (k = 0; k<n; k++)
				if ((r = rand()) % 4) a[i][j][k] = BLACK;
				else {

					switch (r % 3)
					{
					case 0: a[i][j][k] = BRICK; break;
					case 1: a[i][j][k] = WATER; break;
					case 2: a[i][j][k] = STEEL; break;
					}
				}
				doteraj_mape(a);
				return a;

}

*/
void upisi(int ***a)
{
	// Cuva sve mape
	int m = SCREEN_HEIGHT, n = SCREEN_WIDTH;
	int lvl = MAX_LEVELS;
	m = m / UNIT;
	n = n / UNIT;
	int i, j, k;
	FILE *mapice;

	if (SCREEN_HEIGHT == LITTLE_HEIGHT)
		mapice = fopen("gfx/maps/smallmaps.txt", "w");
	else 
		mapice = fopen("gfx/maps/bigmaps.txt", "w");

	for (i = 1; i <= lvl; i++)
		for (j = 0; j<m; j++)
			for (k = 0; k<n; k++)
			{
				fprintf(mapice, " %d", a[i][j][k]);
			}

	fclose(mapice);

}


void ispisi(int ***a)
{
	int i, j, k;
	// Uzima mape iz fajla
	FILE *mapice;
	int m = SCREEN_HEIGHT, n = SCREEN_WIDTH;
	int lvl = MAX_LEVELS;
	m = m / UNIT;
	n = n / UNIT;
	if (SCREEN_HEIGHT == LITTLE_HEIGHT)
		mapice = fopen("gfx/maps/smallmaps.txt","r");
	else
		mapice = fopen("gfx/maps/bigmaps.txt", "r");

	
	for (i = 1; i <= lvl; i++)
		for (j = 0; j<m; j++)
			for (k = 0; k<n; k++)
				fscanf(mapice, "%d", &a[i][j][k]);
	fclose(mapice);
}

int*** getMaps()
{
	int i, j, k, ***a;
	int m = SCREEN_HEIGHT, n = SCREEN_WIDTH;
	int r;
	m = m / UNIT;
	n = n / UNIT;

	
		a = create_map();
		//for (i = 1; i <= 10; i++) random_mapa(a, i);
		ispisi(a);
		created = 1;
	
	return a;

}

int ***getMaps2(int***a) {
	int i, j, k;
	int m = SCREEN_HEIGHT, n = SCREEN_WIDTH;
	int r;
	m = m / UNIT;
	n = n / UNIT;

	if (!a) {
		a = create_map();
		//for (i = 1; i <= 10; i++) random_mapa(a, i);
			ispisi(a);
	
		created = 1;
	}
	return a;

}



void dealok(int ***a)
{
	int i, j, k;
	int m = SCREEN_HEIGHT, n = SCREEN_WIDTH;
	int lvl = MAX_LEVELS;
	m = change_screen;
	n = change_screen > LITTLE_HEIGHT ? BIG_WIDTH: LITTLE_WIDTH;


	m = m / UNIT;
	n = n / UNIT;

	for (i = 1; i <= lvl; i++)
	{
		for (j = 0; j<m; j++) free(a[i][j]);
		free(a[i]);
	}
	free(a);

}
				

void restore()
{
	
	int m = BIG_HEIGHT, n = BIG_WIDTH;
	int lvl = MAX_LEVELS;
	m = m / UNIT;
	n = n / UNIT;
	int ***a;
	int i, j, l;
	FILE *mapice;
	SCORES* scores = NULL;
	unsigned long int *S, k[4] = { 1236,87623,78976,34679 }, score1, score2;
	SCORES *skor;
	char ime1[10], ime2[10];

	skor = malloc(11 * sizeof(SCORES));
	FILE *izlaz, *ulaz, *mapice2;

	/*
	a = malloc(100 * sizeof(int**));
	a[0] = NULL;
	for (l = 1; l<100; l++)
	{
		a[l] = malloc(m * sizeof(int*));
		for (i = 0; i<m; i++)
		{
			a[l][i] = malloc(n * sizeof(int));
			for (j = 0; j<n; j++)
				a[l][i][j] = BLACK;
		}
	}
	

		mapice = fopen("gfx/maps/restore/bigmaps.txt", "r");
		mapice2 = fopen("gfx/maps/bigmaps.txt", "w");
		
	for (i = 1; i <= lvl; i++)
		for (j = 0; j<m; j++)
			for (l = 0; l<n; l++)
			{
				fscanf(mapice, " %d", &a[i][j][l]);
				fprintf(mapice2, " %d", a[i][j][l]);
			}

	fclose(mapice);
	fclose(mapice2);
	mapice2 = NULL;

	for (i = 1; i <= lvl; i++)
	{
		for (j = 0; j<m; j++) free(a[i][j]);
		free(a[i]);
	}
	free(a);

	m = LITTLE_HEIGHT / UNIT;
	n = LITTLE_WIDTH / UNIT;
	
	a = NULL;

	mapice = fopen("gfx/maps/restore/smallmaps.txt", "r");
	a = malloc(100 * sizeof(int**));
	a[0] = NULL;
	for (l = 1; l<100; l++)
	{
		a[l] = malloc(m * sizeof(int*));
		for (i = 0; i<m; i++)
		{
			a[l][i] = malloc(n * sizeof(int));
			for (j = 0; j<n; j++)
				a[l][i][j] = BLACK;
		}
	}
	fclose(mapice);
	mapice2 = fopen("gfx/maps/bigmaps.txt", "w");
	for (i = 1; i <= lvl; i++)
		for (j = 0; j<m; j++)
			for (l = 0; l<n; l++)
			{
				fscanf(mapice, "%d", &a[i][j][l]);
				fprintf(mapice2, "%d", a[i][j][l]);
			}
	fclose(mapice2);
	mapice2 = NULL;



	
	for (i = 1; i <= lvl; i++)
	{
		for (j = 0; j<m; j++) free(a[i][j]);
		free(a[i]);
	}
	free(a);
	a = NULL;
	
	*/
	
	
	
	ulaz = fopen("gfx/scores/restore/izlaz.txt", "r");
	S = malloc(22 * sizeof(unsigned long int));
	for (i = 0; i<10; i += 2)
	{
		for (j = 0; j<22; j += 2)
		{
			fscanf(ulaz, "%lu %lu", &S[j], &S[j + 1]);
			decrypt(&S[j], k);
			if (j == 20) continue;
			ime1[j / 2] = S[j];
			ime2[j / 2] = S[j + 1];
		}
		score1 = S[20];
		score2 = S[21];
		strcpy(skor[i].username, ime1);
		strcpy(skor[i + 1].username, ime2);
		skor[i].score = score1;
		skor[i + 1].score = score2;

	}
	fclose(ulaz);
	lock_scores(skor);


}