
#ifndef utils
#define utils


struct pair {
	int valeur, rang; // la valeur et le rang (MPI) du processus.
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
                                     int nombre_pairs, int nombre_clefs_exposant);

void init_pairs_aleatoire_classe(int **p_pairs_valeurs,
                                 int nombre_pairs, int nombre_clefs_exposant);

struct pair *creer_finger_table(int ma_valeur, int nombre_clefs_exposant,
                                          struct pair *tableau_pairs,
					  int nombre_pairs);

// Obtenir un nombre basé sur le nombre de microsecondes dans la seconde actuelle
// sert notamment si on appelle plusieurs fois srand() en une seconde.
int get_rand_time(void);

// Pretty-print d'une finger table.
void afficher_finger_table(int valeur_ce_pair, int rang_ce_pair,
                           int nombre_clefs_exposant, struct pair *f_table);
int trouver_index_responsable(struct pair *finger_table,
                              int len_ftable, int clef, int ma_valeur);

int appartient_arc_oriente_large(int a, int b, int k, int N);

int appartient_arc_oriente_strict(int a, int b, int k, int N);

#endif