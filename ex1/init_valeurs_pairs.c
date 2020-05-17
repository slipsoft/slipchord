#include <stdio.h>
#include <mpi.h>
#include <stdlib.h> // pour rand et srand
#include "init_valeurs_pairs.h"


int qsort_compare_int(const void* a, const void* b)
{
	return *((int*) a) - *((int*) b);
}

/* Initialisation des pairs selon l'énoncé du TD CHORD.
*/
void init_pairs_TD(int **p_pairs_valeurs,
                   int *p_nombre_pairs, int *p_nombre_clefs,
                   int *p_nombre_clefs_exposant)
{
	
	int nombre_pairs = 10;
	int nombre_clefs_exposant = 6;
	int nombre_clefs = 1 << nombre_clefs_exposant;
	
	int *pairs_valeurs = malloc(sizeof(int) * nombre_pairs);
	pairs_valeurs[0] = 2;
	pairs_valeurs[1] = 7;
	pairs_valeurs[2] = 13;
	pairs_valeurs[3] = 14;
	pairs_valeurs[4] = 21;
	pairs_valeurs[5] = 38;
	pairs_valeurs[6] = 42;
	pairs_valeurs[7] = 48;
	pairs_valeurs[8] = 51;
	pairs_valeurs[9] = 59;
	
	// Retours
	*p_pairs_valeurs = pairs_valeurs;
	*p_nombre_pairs = nombre_pairs;
	*p_nombre_clefs = nombre_clefs;
	*p_nombre_clefs_exposant = nombre_clefs_exposant;
}


/* Initialisation des pairs d'une manière aléatoire.
*/
void init_pairs_aleatoire(int **p_pairs_valeurs,
                         int *p_nombre_pairs, int *p_nombre_clefs,
                         int *p_nombre_clefs_exposant)
{
	
	int nombre_pairs;
	int nombre_clefs;
	int nombre_clefs_exposant = 6;
	nombre_clefs = 1 << nombre_clefs_exposant;
	
	MPI_Comm_size(MPI_COMM_WORLD, &nombre_pairs);
	nombre_pairs--; // sans le processus simulateur
	
	
	// Valeurs associées aux pairs
	int *pairs_valeurs = malloc(sizeof(int) * nombre_pairs);
	
	// Nombre de pairs correctement créés, pour évider les collisions
	//int nb_pairs_valides = 0;
	
	// Attribution d'une valeur unique à chaque pair
	// (ici, la complexité est en n²)
	for (int pair_index = 0; pair_index < nombre_pairs; ++pair_index) {
		
		int valeur;
		do {
			valeur = rand() % nombre_clefs;
			
			// Vérification que la valeur n'est pas déja prise
			// i.e qu'il n'y a pas collision
			for (int i = 0; i < pair_index; ++i) {
				if (valeur == pairs_valeurs[i]) {
					valeur = -1;
					break;
				}
			}
		} while (valeur == -1);
		
		pairs_valeurs[pair_index] = valeur;
		//printf("pair %d  valeur %d \n", pair_index, valeur);
	}
	
	// Il faut désormais classer les pairs par ordre croissant
	// J'utilise un qsort (complexité n.log(n)) et non un algo "postal"
	// même s'il y a un nombre fini de cases, car il y a beaucoup plus
	// de cases possibles que de pairs.
	
	qsort(pairs_valeurs, nombre_pairs, sizeof(int), qsort_compare_int);
	
	for (int pair_index = 0; pair_index < nombre_pairs; ++pair_index) {
		printf("pair %d  valeur %d \n",
			pair_index, pairs_valeurs[pair_index]);
	}
	
	// Retours
	*p_pairs_valeurs = pairs_valeurs;
	*p_nombre_pairs = nombre_pairs;
	*p_nombre_clefs = nombre_clefs;
	*p_nombre_clefs_exposant = nombre_clefs_exposant;
}