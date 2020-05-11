#include <stdio.h>
#include <mpi.h>
//#include <math.h>   // Pour la fonction pow
#include <stdlib.h> // pour rand et srand
#include <time.h>   // pour time


// Elevation à la puissance d'entiers naturels
/*int natural_power(int x, int y)
{
	int result = 1;
	for (int i = 0; i < y; ++y) {
		result *= result;
	}
	return result;
}*/

int compare_int(const void* a, const void* b)
{
	return *((int*) a) - *((int*) b);
}


/* Initialise les finger table des processus.
   
   

void initialisation(int c_exposant, int ) {
	
}*/

/* Initialisation des pairs selon l'énoncé du TD CHORD.
*/
void init_pairs_TD(int **p_pairs_valeurs,
                   int *p_nombre_pairs, int *p_nombre_clefs,
                   int *p_nombre_clefs_exposant) {
	
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
                         int *p_nombre_clefs_exposant) {
	
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
	
	qsort(pairs_valeurs, nombre_pairs, sizeof(int), compare_int);
	
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


// Initialisation de la DHT, envoi aux autres processus de leurs informations.
void simulateur(void)
{
	srand(time(NULL));
	
	// Initialisation du nombre de clefs
	//int nombre_clefs_exposant = 6;
	
	// Un left shift élève à la bonne puissance de 2.
	//int nombre_clefs = 1 << nombre_clefs_exposant;
	
	// Initialisation des valeurs associées aux pairs
	//int nombre_pairs;
	//MPI_Comm_size(MPI_COMM_WORLD, &nombre_pairs);
	//nombre_pairs--;
	
	int *pairs_valeurs;
	int nombre_pairs, nombre_clefs;
	int nombre_clefs_exposant;
	
	//init_pairs_aleatoire(&pairs_valeurs, &nombre_pairs, &nombre_clefs, &nombre_clefs_exposant);
	init_pairs_TD(&pairs_valeurs, &nombre_pairs, &nombre_clefs, &nombre_clefs_exposant);
	
	// TODO Plus tard, gérer les cas d'erreur (nombre_pairs < 2)
	
	printf("nombre_clefs_exposant(%d)  nombre_clefs(%d)"
	       "nombre_pairs(%d) \n",
		nombre_clefs_exposant, nombre_clefs,
		nombre_pairs);
	
	
	
	
	// *** Calcul des finger table de chaque processus ***
	
	// Finger tables de tous les processus
	int **f_tables_pairs = malloc(sizeof(int *) * nombre_pairs);
	
	// Je répète l'opération pour chaque pair
	for (int i_pair = 0; i_pair < nombre_pairs; ++i_pair) {
		
		// Valeur associée au pair d'index i_pair
		int v_pair = pairs_valeurs[i_pair];
		
		// Finger table de ce processus (d'index i_pair)
		int *f_table = malloc(sizeof(int) * nombre_clefs_exposant);
		f_tables_pairs[i_pair] = f_table;
		
		
		// Je détermine toutes les associations clef - pair en charge de cette clef.
		for (int i_clef = 0; i_clef < nombre_clefs_exposant; ++i_clef) {
			
			// Valeur de la clef à l'emplacement i_clef de la finger_table
			int v_clef = (v_pair + (1 << i_clef)) % nombre_clefs;
			
			// Je trouve le pair en charge de cette clef
			int i_pair_associe;
			int distance_min = -1;
			// Le pair associé est le plus petit pair plus grand ou
			// égal à la clef. I.e. je prends la plus petite
			// distance entre v_clef et un pair, dans le sens
			// contraire au sens trigonométrique (distance donc
			// comprise entre 0 et nombre_clefs-1).
			for (int p_ind = 0; p_ind < nombre_pairs; ++p_ind) {
				int p_val = pairs_valeurs[p_ind];
				// Si la valeur du pair est plus petite que la
				// clef, je rajoute un tour.
				if (p_val < v_clef) p_val += nombre_clefs;
				// La valeur du pair est donc strictement
				// supérieure à la valeur de la clef.
				int dist = p_val - v_clef;
				if ((distance_min == -1)
				|| ((distance_min != -1) && (dist < distance_min))) {
					distance_min = dist;
					i_pair_associe = p_ind;
				}
			}
			
			// valeur du pair en charge de cette clef
			int v_pair_associe = pairs_valeurs[i_pair_associe];
			
			f_table[i_clef] = v_pair_associe;
		}
	}
	
	// Affichage des finger table :
	
	for (int i_pair = 0; i_pair < nombre_pairs; ++i_pair) {
		int *f_table = f_tables_pairs[i_pair];
		int v_pair = pairs_valeurs[i_pair];
		printf("--- Pair index %d  valeur %d ---\n", i_pair, v_pair);
		for (int i_clef = 0; i_clef < nombre_clefs_exposant; ++i_clef) {
			int v_clef = (v_pair + (1 << i_clef)) % nombre_clefs;
			int v_pair_responsable = f_table[i_clef];
			printf("clef %d  valeur %d  pair responsable %d\n",
			       i_clef, v_clef, v_pair_responsable);
			
		}
	}
	
	
	
	
	// reste :
	// envoyer les valeurs aux processus convcernés
	
	
	for (int i_pair = 0; i_pair < nombre_pairs; ++i_pair) {
		int *f_table = f_tables_pairs[i_pair];
		free(f_table);
	}
	
	free(f_tables_pairs);
	free(pairs_valeurs);
}







int main(int argc, char *argv[])
{
	int my_rank;
	int nb_proc;
	int mon_id;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	
	mon_id = my_rank;

	if (mon_id == 0) {
		simulateur();
	}

	//printf("Processus %d sur %d : Hello\n", my_rank, nb_proc);

	MPI_Finalize();
	return 0;
}