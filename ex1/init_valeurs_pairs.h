
#ifndef INIT_VALEURS_PAIRS
#define INIT_VALEURS_PAIRS

/* Initialisation des pairs selon l'énoncé du TD CHORD. */
void init_pairs_TD(int **p_pairs_valeurs,
                   int *p_nombre_pairs, int *p_nombre_clefs,
                   int *p_nombre_clefs_exposant);

/* Initialisation aléatoire des pairs (en fonction du nombre de processus MPI). */
void init_pairs_aleatoire(int **p_pairs_valeurs,
                         int *p_nombre_pairs, int *p_nombre_clefs,
                         int *p_nombre_clefs_exposant);

#endif