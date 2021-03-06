#include <stdio.h>
#include <mpi.h>
#include <stdlib.h> // pour rand et srand
#include <time.h>
#include "utils.h"


int qsort_compare_int(const void* a, const void* b)
{
	return *((int*) a) - *((int*) b);
}

int qsort_compare_pair(const void* a, const void* b)
{
	return ((struct pair *) a)->valeur - ((struct pair *) b)->valeur;
}

/**
 * Créée un tableu de struct pair à partir d'une liste de valeurs chord.
 */
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

/**
 * Créée la finger table d'un pair à partir d'un tableau de pairs classé selon
 * leur valeur chord. Cette Fingertable est en fait une liste de pairs afin de
 * conserver une correspondance entre les id MPI et les valeurs chord des
 * noeuds.
 * @param ma_valeur valeur chord du noeud courant.
 * @param nombre_clefs_exposant la puissance de 2 du nombre de clés.
 * @param tableau_classe_pairs la table des pairs du noeud courrant. classé par valeur chord.
 * @param nombre_pairs le nombre pairs dans la table des pairs.
 */
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

// Trouver l'index (dans ma finger table) du pair à qui envoyer un message récursif 
// -1 signifie que le pair en charge de la clef est mon successeur.
int trouver_index_responsable(struct pair *finger_table, int len_ftable, int clef, int ma_valeur)
{
	// Je recherche le plus grand pair dont la clef est strictement plus petite que clef.
	// Comme en TD, je prend le pair d'index le plus élevé dans ma finger table (donc le plus
	// grand) vérifiant "clef appartient à l'intervalle ouvert
	// entre (valeur du pair dans la finger_table) et (valeur du pair actuel)"
	if (clef == ma_valeur) return -2;
	int index_retenu = -1;
	int nb_clefs = (1 << len_ftable);
	
	for (int i = 0; i < len_ftable; i++) {
		int v_pair = finger_table[i].valeur;
		//int clef_dans_intervalle = appartient_arc_oriente_strict(ma_valeur, v_pair, clef, nb_clefs);
		int clef_dans_intervalle = appartient_arc_oriente_strict(v_pair, ma_valeur, clef, nb_clefs);
		//clef_dans_intervalle = clef_dans_intervalle || (clef == ma_valeur);
		
		// Pour le débug, affiche si la clef est dans l'intervalle
		// printf("p %d  [%d] -> %d  ds_int %d  [%d,%d] c %d\n",
		// ma_valeur, i, v_pair, clef_dans_intervalle, ma_valeur, v_pair, clef);
		
		if (clef_dans_intervalle) {
			index_retenu = i;
		}
	}
	
	// -1 signifie que le pair en charge de la clef est mon successeur. 
	/*if (index_retenu != -1) {
		if (finger_table[index_retenu].valeur == finger_table[0].valeur) {
			index_retenu = -1;
		}
	}*/
	
	return index_retenu;
}


int appartient_arc_oriente_large(int a, int b, int k, int N)
{
	if (a == b) return 0;
	if (k == a) return 1;
	if (k == b) return 1;
	return appartient_arc_oriente_strict(a, b, k, N);
}

int appartient_arc_oriente_strict(int a, int b, int k, int N)
{
	k = k % N;
	// Cas simple, comme dans les entiers naturels
	if (a <= b) {
		if ( (a < k) && (k < b) ) return 1;
		return 0;
	} else {
		int entre_a_et_zero = (k > a); // k <= N de base
		int entre_b_et_zero = (k < b); // k >= 0 de base
		if (entre_a_et_zero || entre_b_et_zero) return 1;
		return 0;
	}
}