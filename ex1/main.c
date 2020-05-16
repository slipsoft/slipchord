#include <stdio.h>
#include <mpi.h>
//#include <math.h>   // Pour la fonction pow
#include <stdlib.h> // pour rand et srand
#include <time.h>   // pour time

#define TAG_INIT 1 // envoi initial des infos aux pairs
#define TAG_INITACK 2 // acquittement des infos par les pairs au processus init
#define TAG_RECHERCHE_INIT 3 // recherche d'une clef : demande depuis le processus init
#define TAG_RECHERCHE_PAIR 4 // recherche d'une clef : demande depuis un pair
#define TAG_TERM 5 // se terminer

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


struct ftable_element {
	int valeur, rang; // la valeur et le rang du processus.
};

void simulateur(void);
void pair_classique(void);
int b_moins_a(int a, int b, int k);
int trouver_index_responsable(struct ftable_element *finger_table, int len_ftable, int clef, int ma_valeur);
void print_log(const char *msg_type, const char *fct_name, const int line, const char *str);


int qsort_compare_int(const void* a, const void* b)
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

// TODO si j'ai le temps : vérifier les finger tables avec le TD, via une fonction.
/*void verification_finger_table_td(int **f_tables_pairs) {
	
}*/


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
	struct ftable_element **f_tables_pairs = malloc(sizeof(struct ftable_element *) * nombre_pairs);
	
	// Je répète l'opération pour chaque pair
	for (int i_pair = 0; i_pair < nombre_pairs; ++i_pair) {
		
		// Valeur associée au pair d'index i_pair
		int v_pair = pairs_valeurs[i_pair];
		
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
			   comprise entre 0 et nombre_clefs-1). */
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
		}
	}
	
	// Affichage des finger table :
	
	for (int i_pair = 0; i_pair < nombre_pairs; ++i_pair) {
		struct ftable_element *f_table = f_tables_pairs[i_pair];
		int v_pair = pairs_valeurs[i_pair];
		//printf("--- Pair index %d  valeur %d ---\n", i_pair, v_pair);
		/*for (int i_clef = 0; i_clef < nombre_clefs_exposant; ++i_clef) {
			int v_clef = (v_pair + (1 << i_clef)) % nombre_clefs;
			int v_pair_responsable = f_table[i_clef].valeur;
			int r_pair_responsable = f_table[i_clef].rang;
			// printf("clef %d  valeur %d  pair responsable %d  (rang %d)\n",
			// i_clef, v_clef, v_pair_responsable, r_pair_responsable);
		}*/
		
		// test de qui est responsable de quelle valeur :
		int vtest = 4;
		int p_in = trouver_index_responsable(f_table, nombre_clefs_exposant, vtest, v_pair);
		if (p_in != -1) {
			p_in = f_table[p_in].valeur;
		}
		printf("Pair %d  (%d -> %d) \n", v_pair, vtest, p_in);
		//printf("Responsable de %d   :   %d\n", vtest, p_in);
	}
	
	// Envoi des valeurs aux autres processus
	
	for (int i_pair = 0; i_pair < nombre_pairs; ++i_pair) {
		struct ftable_element *f_table = f_tables_pairs[i_pair];
		int v_pair = pairs_valeurs[i_pair];
		int rang_pair = i_pair + 1;
		// Envoi de l'exposant déterminant le nombre de clefs de la DHT
		MPI_Send(&nombre_clefs_exposant, 1, MPI_INT, rang_pair, TAG_INIT, MPI_COMM_WORLD);
		// Envoi de la valeur associée
		MPI_Send(&v_pair, 1, MPI_INT, rang_pair, TAG_INIT, MPI_COMM_WORLD);
		//MPI_Send(&i_pair, 1, MPI_INT, rang_pair, TAG_INIT, MPI_COMM_WORLD);
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
	
	printf("SIMULATEUR OK TERMINE\n");
	
	// Recherche d'une clef par un pair pris au hasard
	
	int rang_pair = 1 + (rand() % (nombre_pairs - 1));
	int valeur_recherchee = rand() % nombre_clefs;
	
	MPI_Send(&valeur_recherchee, 1, MPI_INT, rang_pair, TAG_RECHERCHE_INIT, MPI_COMM_WORLD);
	
	
	// Puis Q2, Q3
	
	
	for (int i_pair = 0; i_pair < nombre_pairs; ++i_pair) {
		struct ftable_element *f_table = f_tables_pairs[i_pair];
		free(f_table);
	}
	
	free(f_tables_pairs);
	free(pairs_valeurs);
}


void pair_classique(void)
{
	int my_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	// Réception de ses informations
	int valeur_associee;
	int nombre_clefs_exposant, nombre_clefs;
	MPI_Status status;
	
	MPI_Recv(&nombre_clefs_exposant, 1, MPI_INT, RANG_SIMULATEUR, TAG_INIT, MPI_COMM_WORLD, &status);
	nombre_clefs = 1 << nombre_clefs_exposant;
	MPI_Recv(&valeur_associee, 1, MPI_INT, RANG_SIMULATEUR, TAG_INIT, MPI_COMM_WORLD, &status);
	
	struct ftable_element *finger_table = malloc(sizeof(struct ftable_element) * nombre_clefs_exposant);
	
	//printf("--- Pair index %d  valeur %d --- rcv\n", my_rank-1, valeur_associee);
	// Réception de la finger table
	for (int i_clef = 0; i_clef < nombre_clefs_exposant; ++i_clef) {
		// Valeur de la clef à l'index i_clef
		int v_clef = (valeur_associee + (1 << i_clef)) % nombre_clefs;
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
	/*
	while (continuer) {
		int valeur_clef;
		// Attente de la réception d'un message
		MPI_Recv(&valeur_clef, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		
		// Le système se termine
		if (status.MPI_TAG == TAG_TERM) {
			continuer = 0;
			continue;
		}
		
		// Le processus init me demande de rechercher une clef
		if (status.MPI_TAG == TAG_RECHERCHE_INIT) {
			// Je recherche le processus à qui envoyer ma demande
			
			printf("Init me demande de rechercher la clef %d\n", valeur_clef);
			
			
			
		}
		
	}
	*/
	
	
	
	
	free(finger_table);
}

int a_moins_b(int a, int b, int k) {
	return - b_moins_a(a, b, k);
}

int a_inf_b(int a, int b, int k) {
	int res = b_moins_a(a, b, k);
	
	if (res > 0) return 1;
	return 0;
}


// retourne le résultat de la soustraction entre a et b (cyclique)
int b_moins_a(int a, int b, int k)
{
	// soustraction sur les entiers positive
	if ( (b - a) > 0 ) {
		// résultat >= 0 : b plus grand au sens cyclique
		if ( (b - a) < k / 2 ) return b - a;
		// résultat <= 0 : a plus grand au sens cyclique
		return (b - a) - k; // (distance bien indéfieure à k/2 car ((b-a) >= k/2))
	} else /* (b - a) <= 0 */ {
		// résultat <= 0 : a plus grand au sens cyclique
		if ( (a - b) < k / 2 ) return b - a;
		// résultat >= 0 : b plus grand au sens cyclique
		return k + (b - a);
	}
}

// Regarde si k appartient à ]a, b]
int appartient_ouvert_ferme(int a, int b, int k, int N) {
	if (a < b) return ((k > a) && (k <= b));
	return (((k >= 0) && (k < b)) || ((k >= a) && (k < N)));
}

// Trouver l'index du pair à qui envoyer un message récursif (dans ma finger table)
// -1 signifie que le pair en charge de la clef est mon successeur.
int trouver_index_responsable(struct ftable_element *finger_table, int len_ftable, int clef, int ma_valeur)
{
	// Je recherche le plus grand pair dont la clef est strictement plus petite que clef.
	int pair_index = -1;
	
	// les pairs sont ordonnés par ordre croissant (en fonction des 2 puissance i)
	for (int i = 0; i < len_ftable; ++i) {
		struct ftable_element *pair = &finger_table[i];
		int v_pair = pair->valeur;
		/*int v_ppair = ma_valeur + (1 << i);
		
		if (v_pair == v_ppair) {
			info("Valeurs identiques\n");
		} else {
			printf("Valeurs différentes : %d et %d\n", v_pair, v_ppair);
		}*/
		
		//printf("Nb clefs : %d \n", 1 << len_ftable);
		
		// Je dois avoir : clef > v_pair  et  clef <= 
		// appartient_ouvert_ferme
		int nb_clef = (1 << len_ftable);
		/*if (nb_clef != 64) error("Pas bonne clef");
		int c1 = a_moins_b(clef, pair->valeur, nb_clef);
		int c2 = a_moins_b(clef, ma_valeur, nb_clef);
		*/
		//int dist_k_j = 
		//int op = pair->valeur - clef;
		/*if ((i == 0) && (op < 0)) {
			
		}*/
		
		int op = a_inf_b(clef, pair->valeur, nb_clef);
		printf("%d  inf?  %d  ->  %d\n", clef, pair->valeur, op);
		
		//int cond = appartient_ouvert_ferme(ma_valeur, v_pair, clef, nb_clef);
		
		//(c1 > 0) && (c2 <= 0)
		if ( op ) {
			// ainsi, je prends le plus grand pair strictement inférieur à la clef
			pair_index = i;
			//printf("%d  ok  (%d)\n", pair->valeur, clef);
		}
		
		
	}
	
	return pair_index;
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