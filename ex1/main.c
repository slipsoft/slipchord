#include <stdio.h>
#include <mpi.h>
//#include <math.h>   // Pour la fonction pow
#include <stdlib.h> // pour rand et srand
#include <time.h>   // pour time
#include "init_valeurs_pairs.h"

#define TAG_INIT 1 // envoi initial des infos aux pairs
#define TAG_INITACK 2 // acquittement des infos par les pairs au processus init
#define TAG_RECHERCHE_INIT 3 // recherche d'une clef : demande depuis le processus init
#define TAG_RECHERCHE_PAIR 4 // recherche d'une clef : demande depuis un pair
#define TAG_RECHERCHE_TROUVE 5 // recherche d'une clef : le responsable envoie que c'est lui
#define TAG_TERM 6 // se terminer
#define TAG_TERM_ACK 7 // acquittement de la terminaison d'un processus

#define RANG_SIMULATEUR 0

// Afficher un message d'erreur. Ne fonctionne qu'après MPI_Init()
#define error(str) print_log("ERREUR", __func__, __LINE__, str)

// Afficher un message. Ne fonctionne qu'après MPI_Init()
#define info(str, ...) print_log(NULL, __func__, __LINE__, str)



// Elevation à la puissance d'entiers naturels
/*int natural_power(int x, int y)
{
	int result = 1;
	for (int i = 0; i < y; ++y) {
		result *= result;
	}
	return result;
}*/




void simulateur(void);
void pair_classique(void);
int b_moins_a(int a, int b, int k);
int trouver_index_responsable_nico(struct ftable_element *finger_table, int len_ftable, int clef, int ma_valeur);
int trouver_index_responsable_sylv(struct ftable_element *finger_table, int len_ftable, int clef, int ma_valeur);
int trouver_index_responsable(struct ftable_element *finger_table, int len_ftable, int clef, int ma_valeur);
void print_log(const char *msg_type, const char *fct_name, const int line, const char *str);
int trouver_index_res(struct ftable_element *finger_table, int len_ftable, int clef, int ma_valeur);

int appartient_arc_oriente_large(int a, int b, int k, int N);
int appartient_arc_oriente_strict(int a, int b, int k, int N);


// TODO si j'ai le temps : vérifier les finger tables avec le TD, via une fonction.
/*void verification_finger_table_td(int **f_tables_pairs) {
	
}*/

/*
int qsort_compare_int(const void* a, const void* b)
{
	return *((int*) a) - *((int*) b);
}

// Initialisation des pairs selon l'énoncé du TD CHORD.
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
}*/

// Initialisation de la DHT, envoi aux autres processus de leurs informations.
void simulateur(void)
{
	time_t valeur_seed = time(NULL);
	//time_t valeur_seed = 1589735441;
	srand(valeur_seed);
	
	printf("VALEUR DE LA SEED ACTUELLE : %ld\n\n", valeur_seed);
	
	// Initialisation du nombre de clefs
	//int nombre_clefs_exposant = 6;
	
	// Un left shift élève à la bonne puissance de 2.
	//int nombre_clefs = 1 << nombre_clefs_exposant;
	
	// Initialisation des valeurs associées aux pairs
	
	int *pairs_valeurs;
	int nombre_pairs, nombre_clefs;
	int nombre_clefs_exposant;
	
	init_pairs_aleatoire_classe(&pairs_valeurs, &nombre_pairs, &nombre_clefs, &nombre_clefs_exposant);
	//init_pairs_TD(&pairs_valeurs, &nombre_pairs, &nombre_clefs, &nombre_clefs_exposant);
	
	// TODO Plus tard, gérer les cas d'erreur (nombre_pairs < 2)
	
	printf("nombre_clefs_exposant(%d)  nombre_clefs(%d)"
	       "nombre_pairs(%d) \n",
		nombre_clefs_exposant, nombre_clefs,
		nombre_pairs);
	
	printf("\n------- Pairs de la DHT -------\n");
	
	// Affichage des pairs
	for (int i_pair = 0; i_pair < nombre_pairs; ++i_pair) {
		printf(" - %d", pairs_valeurs[i_pair]);
	}
	printf(" - \n");
	
	// *** Calcul des finger table de chaque processus ***
	
	// Finger tables de tous les processus
	struct ftable_element **f_tables_pairs = malloc(sizeof(struct ftable_element *) * nombre_pairs);
	
	// Je répète l'opération pour chaque pair
	for (int i_pair = 0; i_pair < nombre_pairs; ++i_pair) {
		
		// Valeur associée au pair d'index i_pair
		int v_pair = pairs_valeurs[i_pair];
		
		struct ftable_element *f_table =
		creer_finger_table(v_pair, nombre_clefs_exposant, pairs_valeurs, nombre_pairs);
		
		f_tables_pairs[i_pair] = f_table;
		/*
		// Finger table de ce processus (d'index i_pair)
		struct ftable_element *f_table = malloc(sizeof(struct ftable_element) * nombre_clefs_exposant);
		f_tables_pairs[i_pair] = f_table;
		
		
		// Je détermine toutes les associations clef - pair en charge de cette clef.
		for (int i_clef = 0; i_clef < nombre_clefs_exposant; ++i_clef) {
			
			// Valeur de la clef à l'emplacement i_clef de la finger_table
			int v_clef = (v_pair + (1 << i_clef)) % nombre_clefs;
			
			// Je trouve le pair en charge de cette clef
			int i_pair_associe;
			int distance_min = -1;
			
			/* Le pair associé est le plus petit pair plus grand ou égal à la clef.
			   i.e. je prends la plus petite distance entre v_clef et un pair,
			   dans le sens contraire au sens trigonométrique (distance donc
			   comprise entre 0 et nombre_clefs-1). * /
			for (int p_ind = 0; p_ind < nombre_pairs; ++p_ind) {
				int p_val = pairs_valeurs[p_ind];
				
				// Si la valeur du pair est plus petite que la clef, je rajoute un tour.
				if (p_val < v_clef) p_val += nombre_clefs;
				
				// La valeur du pair est donc supérieure ou égale à la valeur de la clef.
				int dist = p_val - v_clef;
				if ((distance_min == -1)
				|| ((distance_min != -1) && (dist < distance_min))) {
					distance_min = dist;
					i_pair_associe = p_ind;
				}
			}
			
			// valeur du pair en charge de cette clef
			int v_pair_associe = pairs_valeurs[i_pair_associe];
			
			f_table[i_clef].valeur = v_pair_associe;
			f_table[i_clef].rang = i_pair_associe + 1; // le processus 0 est le simulateur
		}*/
	}
	
	// Affichage des finger table :
	//int vtest = 7;
	
	printf("\n------- Finger tables des pairs -------\n");
	
	for (int i_pair = 0; i_pair < nombre_pairs; ++i_pair) {
		struct ftable_element *f_table = f_tables_pairs[i_pair];
		int v_pair = pairs_valeurs[i_pair];
		printf("\n--- Finger table du pair %d (index %d) ---\n", v_pair, i_pair);
		for (int i_clef = 0; i_clef < nombre_clefs_exposant; ++i_clef) {
			int v_clef = (v_pair + (1 << i_clef)) % nombre_clefs;
			int v_pair_responsable = f_table[i_clef].valeur;
			int r_pair_responsable = f_table[i_clef].rang;
			
			
			printf("   [%d]  clef(%3d)  pair(%3d)   rang_MPI(%2d)\n",
			i_clef, v_clef, v_pair_responsable, r_pair_responsable);
		}
	}
	printf("\n\n");
	
	// Envoi des valeurs aux autres processus
	
	for (int i_pair = 0; i_pair < nombre_pairs; ++i_pair) {
		struct ftable_element *f_table = f_tables_pairs[i_pair];
		int v_pair = pairs_valeurs[i_pair];
		int rang_pair = i_pair + 1;
		
		// Envoi de l'exposant déterminant le nombre de clefs de la DHT
		MPI_Send(&nombre_clefs_exposant, 1, MPI_INT, rang_pair, TAG_INIT, MPI_COMM_WORLD);
		
		// Envoi de la valeur associée
		MPI_Send(&v_pair, 1, MPI_INT, rang_pair, TAG_INIT, MPI_COMM_WORLD);
		// Envoi de la finger table
		for (int i_clef = 0; i_clef < nombre_clefs_exposant; ++i_clef) {
			MPI_Send(&f_table[i_clef].valeur, 1, MPI_INT, rang_pair, TAG_INIT, MPI_COMM_WORLD);
			MPI_Send(&f_table[i_clef].rang, 1, MPI_INT, rang_pair, TAG_INIT, MPI_COMM_WORLD);
		}
	}
	
	// Attente de l'ack de tous les pairs
	for (int i_pair = 0; i_pair < nombre_pairs; ++i_pair) {
		int ack;
		int rang_pair = i_pair + 1;
		MPI_Recv(&ack, 1, MPI_INT, rang_pair, TAG_INITACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	
	
	// Recherche d'une clef par un pair pris au hasard
	
	int rang_pair = 1 + (rand() % (nombre_pairs - 1));
	int valeur_recherchee = rand() % nombre_clefs;
	
	MPI_Send(&valeur_recherchee, 1, MPI_INT, rang_pair, TAG_RECHERCHE_INIT, MPI_COMM_WORLD);
	
	int valeur_pair_responsable;
	// Attente de la réponse du pair
	MPI_Recv(&valeur_pair_responsable, 1, MPI_INT, rang_pair, TAG_RECHERCHE_TROUVE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	
	
	// Envoi du message de terminaison
	for (int i_pair = 0; i_pair < nombre_pairs; ++i_pair) {
		int ack;
		int rang_pair = i_pair + 1;
		MPI_Send(&ack, 1, MPI_INT, rang_pair, TAG_TERM, MPI_COMM_WORLD);
	}
	
	// Attente de la fin de tous les processus pour afficher le message final
	for (int i_pair = 0; i_pair < nombre_pairs; ++i_pair) {
		int ack;
		int rang_pair = i_pair + 1;
		MPI_Recv(&ack, 1, MPI_INT, rang_pair, TAG_TERM_ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	
	printf("\n-----> Simulateur : le pair en charge de la clef %d est le pair %d.\n\n\n",
	valeur_recherchee, valeur_pair_responsable);
	
	for (int i_pair = 0; i_pair < nombre_pairs; ++i_pair) {
		struct ftable_element *f_table = f_tables_pairs[i_pair];
		free(f_table);
	}
	
	free(f_tables_pairs);
	free(pairs_valeurs);
}


void pair_classique(void)
{
	int my_rank, mon_rang;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_rank(MPI_COMM_WORLD, &mon_rang);
	
	// TODO : 
	
	// Réception de ses informations
	int valeur_pair;
	int nombre_clefs_exposant, nombre_clefs;
	MPI_Status status;
	
	MPI_Recv(&nombre_clefs_exposant, 1, MPI_INT, RANG_SIMULATEUR, TAG_INIT, MPI_COMM_WORLD, &status);
	nombre_clefs = 1 << nombre_clefs_exposant;
	MPI_Recv(&valeur_pair, 1, MPI_INT, RANG_SIMULATEUR, TAG_INIT, MPI_COMM_WORLD, &status);
	
	struct ftable_element *finger_table = malloc(sizeof(struct ftable_element) * nombre_clefs_exposant);
	
	//printf("--- Pair index %d  valeur %d --- rcv\n", my_rank-1, valeur_pair);
	// Réception de la finger table
	for (int i_clef = 0; i_clef < nombre_clefs_exposant; ++i_clef) {
		// Valeur de la clef à l'index i_clef
		//int v_clef = (valeur_pair + (1 << i_clef)) % nombre_clefs;
		struct ftable_element *elem = &finger_table[i_clef];
		MPI_Recv(&(elem->valeur), 1, MPI_INT, RANG_SIMULATEUR, TAG_INIT, MPI_COMM_WORLD, &status);
		MPI_Recv(&(elem->rang), 1, MPI_INT, RANG_SIMULATEUR, TAG_INIT, MPI_COMM_WORLD, &status);
		
		//printf("clef %d  valeur %d  pair responsable %d  (rang %d)\n",
		//i_clef, v_clef, elem->valeur, elem->rang);
		
		//int v_pair_responsable = f_table[i_clef];
		//MPI_Send(&v_pair_responsable, 1, MPI_INT, rang_pair, TAG_INIT, MPI_COMM_WORLD);
	}
	
	int ack = 0;
	MPI_Send(&ack, 1, MPI_INT, RANG_SIMULATEUR, TAG_INITACK, MPI_COMM_WORLD);
	
	int continuer = 1;
	
	while (continuer) {
		int valeur_clef;
		// Attente de la réception d'un message
		MPI_Recv(&valeur_clef, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		
		// Le système se termine
		if (status.MPI_TAG == TAG_TERM) {
			continuer = 0;
			MPI_Send(&ack, 1, MPI_INT, RANG_SIMULATEUR, TAG_TERM_ACK, MPI_COMM_WORLD);
			continue;
		}
		
		// à qui renvoyer le message lorsque le pair responsable est trouvé
		int rang_retour_message;
		
		int doit_renvoyer_message;
		
		// Pour factoriser les fonctions, vrai si je dois rechercher un pair
		int doit_rechercher_clef = 0;
		
		// Le processus init me demande de rechercher une clef
		if (status.MPI_TAG == TAG_RECHERCHE_INIT) {
			doit_rechercher_clef = 1;
			doit_renvoyer_message = 0;
			rang_retour_message = mon_rang; // le message devra m'être retourné
			// Je recherche le processus à qui envoyer ma demande
			printf("(pair %d) : Simulateur me demande de rechercher la clef %d\n\n", valeur_pair, valeur_clef);
		}
		
		
		// Un pair me demande de rechercher une clef
		if (status.MPI_TAG == TAG_RECHERCHE_PAIR) {
			doit_rechercher_clef = 1;
			doit_renvoyer_message = 1;
			// Je reçois le rang duquel je vais devenoir recevoir un message
			MPI_Recv(&rang_retour_message, 1, MPI_INT, status.MPI_SOURCE, TAG_RECHERCHE_PAIR, MPI_COMM_WORLD, &status);
		}
		
		
		if (doit_rechercher_clef) {
			int resp_index = trouver_index_responsable(finger_table,
				nombre_clefs_exposant, valeur_clef, valeur_pair);
			
			int resp_valeur;
			int resp_rang;
			
			if (resp_index == -2) {
				// Je suis responsable de la clef
				resp_valeur = valeur_pair;
				resp_rang = mon_rang;
				printf("(pair %d) : je suis responsable de la clef(%d).\n\n", valeur_pair, valeur_clef);
				
			} else if (resp_index == -1) {
				// Mon successeur est responsable de la clef
				resp_valeur = finger_table[0].valeur;
				resp_rang = finger_table[0].rang;
				printf("(pair %d) : mon successeur(%d) est responsable de la clef(%d).\n\n",
				valeur_pair, resp_valeur, valeur_clef);
				
			} else {
				// Il faut que j'envoie une demande à un pair pour trouver le responsable
				printf("(pair %d) : j'envoie la demande au pair %d (recherche de la clef %d).\n\n",
				valeur_pair, finger_table[resp_index].valeur, valeur_clef);
				
				// Envoi de la demande au pair
				int dest = finger_table[resp_index].rang;
				int msg[2];
				MPI_Send(&valeur_clef, 1, MPI_INT, dest, TAG_RECHERCHE_PAIR, MPI_COMM_WORLD);
				
				// Envoi du rang à qui renvoyer le message, une fois le responsable trouvé,
				// pour que le message puisse revenir directement au pair qui a fait la permière demande.
				MPI_Send(&rang_retour_message, 1, MPI_INT, dest, TAG_RECHERCHE_PAIR, MPI_COMM_WORLD);
				
				// Si je suis le processus qui a fait la demande initiale, j'attends la réponse
				// if (rang_retour_message == mon_rang)
				if (status.MPI_TAG == TAG_RECHERCHE_INIT) {
					// Réception de la réponse (du pair dont le successeur est responsable de la clef)
					int msg[2];
					MPI_Recv(&resp_valeur, 1, MPI_INT, MPI_ANY_SOURCE, TAG_RECHERCHE_TROUVE, MPI_COMM_WORLD, &status);
					MPI_Recv(&resp_rang, 1, MPI_INT, status.MPI_SOURCE, TAG_RECHERCHE_TROUVE, MPI_COMM_WORLD, &status);
					printf("(pair %d) : réception du pair responsable de la clef : valeur(%d) rang_MPI(%d).\n\n",
					valeur_pair, resp_valeur, resp_rang);
					
					// Envoi au simulateur (le simulateur va broadcast un message de terminaison)
					MPI_Send(&resp_valeur, 1, MPI_INT, RANG_SIMULATEUR, TAG_RECHERCHE_TROUVE, MPI_COMM_WORLD);
				}
			}
			
			// Si je suis le processus qui a fait la demande initiale,
			// ma demande a été bloquante, du coup là j'envoie un message au processus
			// init.
			//if (status.MPI_TAG == TAG_RECHERCHE_INIT) {
			if (doit_renvoyer_message) {
				
				MPI_Send(&resp_valeur, 1, MPI_INT, rang_retour_message, TAG_RECHERCHE_TROUVE, MPI_COMM_WORLD);
				MPI_Send(&resp_rang, 1, MPI_INT, rang_retour_message, TAG_RECHERCHE_TROUVE, MPI_COMM_WORLD);
			}
			/*if (rang_retour_message != -1) {
				// Envoi du message au pair qui a initialement fait la recherche
				MPI_Send(&resp_valeur, 1, MPI_INT, rang_retour_message, TAG_RECHERCHE_TROUVE, MPI_COMM_WORLD);
			}*/
			
			
			
		}
	}
	
	free(finger_table);
}



// Trouver l'index (dans ma finger table) du pair à qui envoyer un message récursif 
// -1 signifie que le pair en charge de la clef est mon successeur.
int trouver_index_responsable(struct ftable_element *finger_table, int len_ftable, int clef, int ma_valeur)
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
		}/* else {
			
		}*/
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

/*
#define TAG_INIT 1
#define TAG_INITACK 2
#define TAG_RECHERCHE 3
#define TAG_TERM 4
*/

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
	} else {
		pair_classique();
	}

	//printf("Processus %d sur %d : Hello\n", my_rank, nb_proc);

	MPI_Finalize();
	return 0;
}


// Afficher un message. Ne fonctionne qu'après MPI_Init()
void print_log(const char *msg_type, const char *fct_name, const int line, const char *str) {
	int rang;
	MPI_Comm_rank(MPI_COMM_WORLD, &rang);
	if (msg_type != NULL) {
		printf("(de %d) %s : %s (%s l.%d)\n", rang, msg_type, str, fct_name, line);
	} else {
		printf("(de %d) %s (%s l.%d)\n", rang, str, fct_name, line);
	}
}