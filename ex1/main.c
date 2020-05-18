#include <stdio.h>
#include <mpi.h>
#include <stdlib.h> // pour rand et srand
#include <time.h>   // pour time
#include "utils.h"

#define TAG_INIT 1 // envoi initial des infos aux pairs
#define TAG_INITACK 2 // acquittement des infos par les pairs au processus init
#define TAG_RECHERCHE_INIT 3 // recherche d'une clef : demande depuis le processus init
#define TAG_RECHERCHE_PAIR 4 // recherche d'une clef : demande depuis un pair
#define TAG_RECHERCHE_TROUVE 5 // recherche d'une clef : le responsable envoie que c'est lui
#define TAG_TERM 6 // se terminer
#define TAG_TERM_ACK 7 // acquittement de la terminaison d'un processus

int RANG_SIMULATEUR; // défini au début du main, c'est le rang du dernier processus MPI

// Afficher un message d'erreur. Ne fonctionne qu'après MPI_Init()
#define error(str) print_log("ERREUR", __func__, __LINE__, str)

// Afficher un message. Ne fonctionne qu'après MPI_Init()
#define info(str, ...) print_log(NULL, __func__, __LINE__, str)


void simulateur(long cseed);
void pair_classique(void);
void print_log(const char *msg_type, const char *fct_name, const int line, const char *str);


// Affichage de toutes les informations (dont finger table)
int show_full_debug_info = 1;

// Affichage des messages envoyés et reçus par les pairs de la DHT
int show_peers_messages = 1;

// 0 si initialiser en fonction du TD, 1 si initialiser aléatoirement
int dht_aleatoire = 1;


// Initialisation de la DHT, envoi aux autres processus de leurs informations.
void simulateur(long cseed)
{
	srand(cseed);
	
	// L'affichage de la seed qui permet de reproduire l'exécution, en cas de bug
	printf("VALEUR DE LA SEED ACTUELLE : %ld\n", cseed);
	
	
	// Initialisation des valeurs associées aux pairs
	int *pairs_valeurs;
	struct pair *tableau_pairs;
	int nombre_pairs, nombre_clefs;
	int nombre_clefs_exposant;
	
	if (dht_aleatoire) {
		// Initialiser aléatoirement
		nombre_clefs_exposant = 6; //(rand() % 8) + 3;
		MPI_Comm_size(MPI_COMM_WORLD, &nombre_pairs);
		--nombre_pairs;
		nombre_clefs = 1 << nombre_clefs_exposant;
		init_pairs_aleatoire_classe(&pairs_valeurs, nombre_pairs, nombre_clefs_exposant);
	} else {
		// Initialiser en fonction des pairs du TD
		init_pairs_TD(&pairs_valeurs, &nombre_pairs, &nombre_clefs, &nombre_clefs_exposant);
	}
	
	tableau_pairs = creer_tableau_pair(pairs_valeurs, nombre_pairs);
	
	// Affichage des pairs de la DHT
	if (show_peers_messages) {
		printf("nombre_clefs_exposant(%d)  nombre_clefs(%d)  "
		"nombre_pairs(%d) \n",
			nombre_clefs_exposant, nombre_clefs,
			nombre_pairs);
		
		printf("\n------- Pairs de la DHT -------\n");
		
		// Affichage des pairs
		for (int i_pair = 0; i_pair < nombre_pairs; ++i_pair) {
			//printf(" - %d", pairs_valeurs[i_pair]);
			printf(" - %d", tableau_pairs[i_pair].valeur);
		}
		printf(" - \n\n");
	}
	
	// *** Calcul des finger table de chaque processus ***
	
	// Finger tables de tous les processus
	struct pair **f_tables_pairs = malloc(sizeof(struct pair *) * nombre_pairs);
	
	// Je répète l'opération pour chaque pair
	for (int i_pair = 0; i_pair < nombre_pairs; ++i_pair) {
		
		// Valeur associée au pair d'index i_pair
		int v_pair = pairs_valeurs[i_pair];
		
		struct pair *f_table =
		creer_finger_table(v_pair, nombre_clefs_exposant, tableau_pairs, nombre_pairs);
		
		f_tables_pairs[i_pair] = f_table;
	}
	
	// Affichage des finger table :
	//int vtest = 7;
	
	if (show_full_debug_info) {
		printf("------- Finger tables des pairs -------\n");
		
		for (int i_pair = 0; i_pair < nombre_pairs; ++i_pair) {
			struct pair *f_table = f_tables_pairs[i_pair];
			int v_pair = pairs_valeurs[i_pair];
			afficher_finger_table(v_pair, i_pair, nombre_clefs_exposant, f_table);
		}
		printf("\n\n");
	}
	
	// Envoi des valeurs aux autres processus
	
	for (int i_pair = 0; i_pair < nombre_pairs; ++i_pair) {
		struct pair *f_table = f_tables_pairs[i_pair];
		int v_pair = pairs_valeurs[i_pair];
		int rang_pair = i_pair;
		
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
		int rang_pair = i_pair;
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
		int rang_pair = i_pair;
		MPI_Send(&ack, 1, MPI_INT, rang_pair, TAG_TERM, MPI_COMM_WORLD);
	}
	
	// Attente de la fin de tous les processus pour afficher le message final
	for (int i_pair = 0; i_pair < nombre_pairs; ++i_pair) {
		int ack;
		int rang_pair = i_pair;
		MPI_Recv(&ack, 1, MPI_INT, rang_pair, TAG_TERM_ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	
	if (show_full_debug_info) printf("\n");
	//printf("\n-----> Simulateur : le pair en charge de la clef %d est le pair %d.\n\n\n",
	printf("-----> Simulateur : le pair en charge de la clef %d est le pair %d.\n",
	valeur_recherchee, valeur_pair_responsable);
	if (show_full_debug_info) printf("\n\n");
	
	for (int i_pair = 0; i_pair < nombre_pairs; ++i_pair) {
		struct pair *f_table = f_tables_pairs[i_pair];
		free(f_table);
	}
	
	free(tableau_pairs);
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
	int nombre_clefs_exposant;
	MPI_Status status;
	
	MPI_Recv(&nombre_clefs_exposant, 1, MPI_INT, RANG_SIMULATEUR, TAG_INIT, MPI_COMM_WORLD, &status);
	MPI_Recv(&valeur_pair, 1, MPI_INT, RANG_SIMULATEUR, TAG_INIT, MPI_COMM_WORLD, &status);
	
	
	struct pair *finger_table = malloc(sizeof(struct pair) * nombre_clefs_exposant);
	
	// Réception de la finger table
	for (int i_clef = 0; i_clef < nombre_clefs_exposant; ++i_clef) {
		struct pair *elem = &finger_table[i_clef];
		MPI_Recv(&(elem->valeur), 1, MPI_INT, RANG_SIMULATEUR, TAG_INIT, MPI_COMM_WORLD, &status);
		MPI_Recv(&(elem->rang), 1, MPI_INT, RANG_SIMULATEUR, TAG_INIT, MPI_COMM_WORLD, &status);
	}
	
	int ack = 0;
	MPI_Send(&ack, 1, MPI_INT, RANG_SIMULATEUR, TAG_INITACK, MPI_COMM_WORLD);
	
	int continuer = 1;
	
	while (continuer) {
		int valeur_clef;
		// Attente de la réception d'un message
		MPI_Recv(&valeur_clef, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		
		//printf("(pair %d rg %d) : réception...  (tag %d)\n", valeur_pair, mon_rang, status.MPI_TAG);
		
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
			if (show_peers_messages) {
				printf("(pair %d rg %d) : Simulateur me demande de rechercher la clef %d\n\n", valeur_pair, mon_rang, valeur_clef);
			}
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
				if (show_peers_messages) {
					printf("(pair %d rg %d) : je suis responsable de la clef(%d).\n\n", valeur_pair, mon_rang, valeur_clef);
				}
				
			} else if (resp_index == -1) {
				// Mon successeur est responsable de la clef
				resp_valeur = finger_table[0].valeur;
				resp_rang = finger_table[0].rang;
				if (show_peers_messages) {
					printf("(pair %d rg %d) : mon successeur(%d) est responsable de la clef(%d).\n\n",
					valeur_pair, mon_rang, resp_valeur, valeur_clef);
				}
				
			} else {
				doit_renvoyer_message = 0; // ce n'est pas à moi de renvoyer le message, comme je passe le message.
				// Il faut que j'envoie une demande à un pair pour trouver le responsable
				if (show_peers_messages) {
					printf("(pair %d rg %d) : j'envoie la demande au pair %d (rg %d) (recherche de la clef %d).\n\n",
					valeur_pair, mon_rang, finger_table[resp_index].valeur, finger_table[resp_index].rang, valeur_clef);
				}
				
				// Envoi de la demande au pair
				int dest = finger_table[resp_index].rang;
				MPI_Send(&valeur_clef, 1, MPI_INT, dest, TAG_RECHERCHE_PAIR, MPI_COMM_WORLD);
				
				// Envoi du rang à qui renvoyer le message, une fois le responsable trouvé,
				// pour que le message puisse revenir directement au pair qui a fait la permière demande.
				MPI_Send(&rang_retour_message, 1, MPI_INT, dest, TAG_RECHERCHE_PAIR, MPI_COMM_WORLD);
				
				// Si je suis le processus qui a fait la demande initiale, j'attends la réponse
				// if (rang_retour_message == mon_rang)
				if (status.MPI_TAG == TAG_RECHERCHE_INIT) {
					// Réception de la réponse (du pair dont le successeur est responsable de la clef)
					MPI_Recv(&resp_valeur, 1, MPI_INT, MPI_ANY_SOURCE, TAG_RECHERCHE_TROUVE, MPI_COMM_WORLD, &status);
					MPI_Recv(&resp_rang, 1, MPI_INT, status.MPI_SOURCE, TAG_RECHERCHE_TROUVE, MPI_COMM_WORLD, &status);
					if (show_peers_messages) {
						printf("(pair %d rg %d) : réception (de rg %d) du pair responsable de la clef : valeur(%d) rang_MPI(%d).\n\n",
						valeur_pair, mon_rang, status.MPI_SOURCE, resp_valeur, resp_rang);
					}
					
				}
			}
			
			
			
			// Si je suis le processus qui a fait la demande initiale,
			// ma demande a été bloquante, du coup là j'envoie un message au processus
			// init.
			//if (status.MPI_TAG == TAG_RECHERCHE_INIT) {
			if (doit_renvoyer_message) {
				if (show_peers_messages) {
					printf("(pair %d rg %d) : je renvoie le message au demandeur : rang_MPI(%d)  responsable : val(%d) rg(%d).\n\n",
					valeur_pair, mon_rang, rang_retour_message, resp_valeur, resp_rang);
				}
				MPI_Send(&resp_valeur, 1, MPI_INT, rang_retour_message, TAG_RECHERCHE_TROUVE, MPI_COMM_WORLD);
				MPI_Send(&resp_rang, 1, MPI_INT, rang_retour_message, TAG_RECHERCHE_TROUVE, MPI_COMM_WORLD);
			} else {
				// i.e. (status.MPI_TAG == TAG_RECHERCHE_INIT)
				// Envoi au simulateur (le simulateur va broadcast un message de terminaison)
				MPI_Send(&resp_valeur, 1, MPI_INT, RANG_SIMULATEUR, TAG_RECHERCHE_TROUVE, MPI_COMM_WORLD);
			}
			/*if (rang_retour_message != -1) {
				// Envoi du message au pair qui a initialement fait la recherche
				MPI_Send(&resp_valeur, 1, MPI_INT, rang_retour_message, TAG_RECHERCHE_TROUVE, MPI_COMM_WORLD);
			}*/
			
		}
	}
	
	free(finger_table);
}

/*
#define TAG_INIT 1
#define TAG_INITACK 2
#define TAG_RECHERCHE 3
#define TAG_TERM 4
*/

int main(int argc, char *argv[])
{
	int nb_proc;
	int mon_rang;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
	MPI_Comm_rank(MPI_COMM_WORLD,&mon_rang);
	
	RANG_SIMULATEUR = nb_proc - 1;
	
	if (mon_rang == RANG_SIMULATEUR) {
		simulateur(get_rand_time());
	} else {
		pair_classique();
	}
	
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