
#ifndef INIT_VALEURS_PAIRS
#define INIT_VALEURS_PAIRS


struct pair {
	int valeur, rang; // la valeur et le rang du processus.
};

int qsort_compare_int(const void* a, const void* b);
int qsort_compare_pair(const void* a, const void* b);

struct pair *creer_tableau_pair(int *pairs_valeurs, int taille);

/* Initialisation des pairs selon l'énoncé du TD CHORD. */
void init_pairs_TD(int **p_pairs_valeurs,
                   int *p_nombre_pairs, int *p_nombre_clefs,
                   int *p_nombre_clefs_exposant);

/* Initialisation aléatoire des pairs (en fonction du nombre de processus MPI). */
void init_pairs_aleatoire_non_classe(int **p_pairs_valeurs,
                                     int *p_nombre_pairs, int *p_nombre_clefs,
                                     int *p_nombre_clefs_exposant);

void init_pairs_aleatoire_classe(int **p_pairs_valeurs,
                                 int *p_nombre_pairs, int *p_nombre_clefs,
                                 int *p_nombre_clefs_exposant);

struct pair *creer_finger_table(int ma_valeur, int nombre_clefs_exposant,
                                          struct pair *tableau_pairs,
					  int nombre_pairs);



#endif