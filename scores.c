#include <SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include <stdio.h>
#include <string.h>
#include "functions.h"
#include<stdlib.h>


void encrypt(unsigned long int * v, unsigned long int* k) {
	unsigned long int v0 = v[0], v1 = v[1], sum = 0, i;           /* set up */
	unsigned long int delta = 0x9e3779b9;                     /* a key schedule constant */
	unsigned long int k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];   /* cache key */
	for (i = 0; i < 32; i++) {                       /* basic cycle start */
		sum += delta;
		v0 += ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
		v1 += ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
	}                                              /* end cycle */
	v[0] = v0; v[1] = v1;
}

void decrypt(unsigned long int* v, unsigned long int* k) {
	unsigned long int v0 = v[0], v1 = v[1], sum = 0xC6EF3720, i;  /* set up */
	unsigned long int delta = 0x9e3779b9;                     /* a key schedule constant */
	unsigned long int k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];   /* cache key */
	for (i = 0; i<32; i++) {                         /* basic cycle start */
		v1 -= ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
		v0 -= ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
		sum -= delta;
	}                                              /* end cycle */
	v[0] = v0; v[1] = v1;
}

SCORES *starting_scores()
{
	int i, j;
	unsigned long int *S, k[4] = { 1236,87623,78976,34679 }, score1, score2;
	SCORES *skor;
	char ime1[10], ime2[10];

	skor = malloc(11 * sizeof(SCORES));
	FILE *izlaz, *ulaz;


	ulaz = fopen("gfx/scores/izlaz.txt", "r");
	izlaz = fopen("gfx/scores/prepis.txt", "w");
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

		fprintf(izlaz, "%s %lu\n", skor[i].username, skor[i].score);

		fprintf(izlaz, "%s %lu\n", skor[i + 1].username, skor[i + 1].score);
	}
	fclose(ulaz);
	fclose(izlaz);

	return skor;
}

void lock_scores(SCORES *skor)
{
	int i, j;
	unsigned long int *S, k[4] = { 1236,87623,78976,34679 }, score;
	char ime[10];
	FILE *izlaz;

	izlaz = fopen("gfx/scores/izlaz.txt", "w");

	S = malloc(22 * sizeof(unsigned long int));
	for (i = 0; i<10; i += 2)
	{
		for (j = 0; j<20; j += 2)
		{
			S[j] = skor[i].username[j / 2];
			S[j + 1] = skor[i + 1].username[j / 2];
		}
		S[20] = skor[i].score;
		S[21] = skor[i + 1].score;

		for (j = 0; j<22; j += 2)
		{
			encrypt(&S[j], k);
			fprintf(izlaz, "%lu %lu ", S[j], S[j + 1]);
		}
	}

	fclose(izlaz);

}

void sort_scores(SCORES *skor)
{
	int i, j;
	unsigned long int score;
	char ime[10];
	for (i = 0; i<10; i++)
		for (j = i + 1; j<10; j++)
		{
			if (skor[i].score<skor[j].score)
			{
				score = skor[i].score;
				skor[i].score = skor[j].score;
				skor[j].score = score;

				strcpy(ime, skor[i].username);
				strcpy(skor[i].username, skor[j].username);
				strcpy(skor[j].username, ime);
			}
		}
}
