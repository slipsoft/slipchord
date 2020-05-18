#include <stdio.h>
#include <mpi.h>
#include <stdlib.h> // pour rand et srand
#include <time.h>
#include "init_valeurs_pairs.h"


int qsort_compare_int(const void* a, const void* b)
{
	return *((int*) a) - *((int*) b);
}

int qsort_compare_pair(const void* a, const void* b)
{
	return ((struct pair *) a)->valeur - ((struct pair *) b)->valeur;
}

struct pair *creer_tableau_pair(int *pairs_valeurs, int taille)
{
	struct pair *tableau = malloc(sizeof(struct pair) * taille);
	for (int i = 0; i < taille; i++) {
		tableau[i].valeur = pairs_valeurs[i];
		tableau[i].rang = i;
		//printf("creer_tableau_pair [%d] -> val %d  rg %d\n", i, tableau[i].valeur, tableau[i].rang);
	}
	return tableau;
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


// Initialisation des pairs d'une manière aléatoire.
void init_pairs_aleatoire_non_classe(int **p_pairs_valeurs,
                                     int nombre_pairs, int nombre_clefs_exposant)
{
	// Initialisation du nombre de clefs
	// Un left shift élève à la bonne puissance de 2.
	int nombre_clefs = 1 << nombre_clefs_exposant;
	
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
	

	
	
	/*for (int pair_index = 0; pair_index < nombre_pairs; ++pair_index) {
		printf("pair %d  valeur %d \n",
			pair_index, pairs_valeurs[pair_index]);
	}*/
	
	// Retours
	*p_pairs_valeurs = pairs_valeurs;
}



void init_pairs_aleatoire_classe(int **p_pairs_valeurs,
                                 int nombre_pairs, int nombre_clefs_exposant)
{
	init_pairs_aleatoire_non_classe(p_pairs_valeurs, nombre_pairs, nombre_clefs_exposant);
	/* Il faut désormais classer les pairs par ordre croissant.
	   J'utilise un qsort (complexité n.log(n)) et non un algo "postal"
	   même s'il y a un nombre fini de cases, car il y a beaucoup plus
	   de cases possibles que de pairs. */
	qsort(*p_pairs_valeurs, nombre_pairs, sizeof(int), qsort_compare_int);
}


struct pair *creer_finger_table(int ma_valeur, int nombre_clefs_exposant,
                                struct pair *tableau_classe_pairs,
                                int nombre_pairs)
{
	/*printf("creer_finger_table pour %d\n", ma_valeur);
	for (int i = 0; i < nombre_pairs; ++i) {
		struct pair pair = tableau_classe_pairs[i];
		printf("pair[%d rg %d] val %d\n", i, pair.rang, pair.valeur);
	}*/
	
	int nombre_clefs = 1 << nombre_clefs_exposant;
	
	// Finger table de ce processus (d'index i_pair)
	struct pair *f_table = malloc(sizeof(struct pair) * nombre_clefs_exposant);
	// ---- f_tables_pairs[i_pair] = f_table;
	
	
	// Je détermine toutes les associations clef - pair en charge de cette clef.
	for (int i_clef = 0; i_clef < nombre_clefs_exposant; ++i_clef) {
		
		// Valeur de la clef à l'emplacement i_clef de la finger_table
		int v_clef = (ma_valeur + (1 << i_clef)) % nombre_clefs;
		
		// Je trouve le pair en charge de cette clef
		int index_pair_associe;
		int distance_min = -1;
		
		/* Le pair associé est le plus petit pair plus grand ou égal à la clef.
		   i.e. je prends la plus petite distance entre v_clef et un pair,
		   dans le sens contraire au sens trigonométrique (distance donc
		   comprise entre 0 et nombre_clefs-1). */
		for (int i = 0; i < nombre_pairs; ++i) {
			struct pair pair = tableau_classe_pairs[i];
			
			int p_val = pair.valeur;
			
			// Si la valeur du pair est plus petite que la clef, je rajoute un tour.
			
			if (p_val < v_clef) p_val += nombre_clefs;
			
			// La valeur du pair est donc supérieure ou égale à la valeur de la clef.
			int dist = p_val - v_clef;
			if ((distance_min == -1)
			|| ((distance_min != -1) && (dist < distance_min))) {
				distance_min = dist;
				index_pair_associe = i;
			}
		}
		
		f_table[i_clef] = tableau_classe_pairs[index_pair_associe];
	}
	
	return f_table;
}

// Obtenir un nombre basé sur le nombre de microsecondes dans la seconde actuelle
// sert notamment si on appelle plusieurs fois srand() en une seconde.
int get_rand_time(void)
{
	struct timespec tm;
	clock_gettime(CLOCK_MONOTONIC, &tm);
	return tm.tv_nsec;
}

// Pretty-print d'une finger table.
void afficher_finger_table(int valeur_ce_pair, int rang_ce_pair,
                           int nombre_clefs_exposant, struct pair *f_table)
{
	int nombre_clefs = 1 << nombre_clefs_exposant;
	
	printf("\n--- Finger table du pair %d (rang mpi %d) ---\n", valeur_ce_pair, rang_ce_pair);
	for (int i_clef = 0; i_clef < nombre_clefs_exposant; ++i_clef) {
		int v_clef = (valeur_ce_pair + (1 << i_clef)) % nombre_clefs;
		int v_pair_responsable = f_table[i_clef].valeur;
		int r_pair_responsable = f_table[i_clef].rang;
		printf("   [%d]  clef(%3d)  pair(%3d)   rang_MPI(%2d)\n",
		i_clef, v_clef, v_pair_responsable, r_pair_responsable);
	}
}