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

// Initialisation de la DHT, envoi aux autres processus de leurs informations.
void simulateur(void)
{
	srand(time(NULL));
	
	// Initialisation du nombre de clefs
	int nombre_clefs_exposant = 6;
	// Un left shift élève à la bonne puissance de 2.
	int nombre_clefs = 1 << nombre_clefs_exposant;//pow(2, nombre_clefs_exposant);
	
	// Initialisation des valeurs associées aux pairs
	int nombre_pairs;
	MPI_Comm_size(MPI_COMM_WORLD, &nombre_pairs);
	nombre_pairs--;
	
	// TODO Plus tard, gérer les cas d'erreur (nombre_pairs < 2)
	
	printf("nombre_clefs_exposant(%d)  nombre_clefs(%d)"
	       "nombre_pairs(%d) \n",
		nombre_clefs_exposant, nombre_clefs,
		nombre_pairs);
	
	// Valaurs associées aux pairs
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
		printf("pair %d  valeur %d \n", pair_index, pairs_valeurs[pair_index]);
	}
	
	// reste :
	// envoyer les valeurs aux processus convcernés
	
	
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