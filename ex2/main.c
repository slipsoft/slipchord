#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include "utils.h"
#include "../ex1/utils.h"

//************   TAGS
#define INIT          0 // Message to init the peertable
#define DONE          1 // Message to propagate the peertable
#define VALUE         2 // Value sent by the simulator
#define KEYEXP        3 // Key_exposant sent by the simulator

//************   VARIABLES MPI

// total process number
int NB;

// Current process mpi id
int rank;

// Current Process chord value
int value;

// Nb key exposant
int nb_keys_exp;

// Voisin de droite, je n'utilise pas le caractère bidirectionnel de l'anneau.
int right;

// Processus toujours actif
int running = 1;

// Positionné à 1 si je suis un initiateur
int initiator;

// Processus simulateur
void simulator()
{
	int *peertable;
	int nb_peers = NB;
	int nb_keys_exp = 6;
	init_pairs_aleatoire_non_classe(&peertable, nb_peers, nb_keys_exp);
	
	
	printf("\n------- Pairs (non classés) de la DHT -------\n");
	
	// Affichage des pairs
	for (int i = 0; i < nb_peers; ++i) {
		printf(" - %d", peertable[i]);
	}
	printf(" - \n\n");
	
	for (int i = 0; i < nb_peers; i++) {
		send_message(i, VALUE, &peertable[i], 1);
		send_message(i, KEYEXP, &nb_keys_exp, 1);
	}
	
	free(peertable);
}

// Processus classique CHORD
void peer()
{
	MPI_Status status;
	
	// Taille du jeton 
	int payload_size = NB + 1;
	
	/* Contenu du message : tableau contenant les N valeurs des pairs
	   ainsi que l'identifiant du jeton (valeur N+1). */
	int *payload = malloc(sizeof(int) * payload_size);
	
	// Identifiant du jeton
	int token_index = NB;
	
	struct pair *peer_table;
	
	/* Si je suis initiateur et que mon jeton a survécu
	   (pour détruire le massage à la fin) */
	int initiateur_gagnant = 0;
	
	// Aléatoire
	srand(getpid());

	// Pair de gauche
	//left = (rank + NB - 1) % NB;
	
	// Pair de droite
	right = (rank + 1) % NB;
	
	// Suis-je initiateur ?
	initiator = rand() % 2;
	
	// Réception de ma valeur, et du nombre de clefs du système (2 puissance nb_keys_exp)
	MPI_Recv(&value, 1, MPI_INT, NB, VALUE, MPI_COMM_WORLD, &status);
	MPI_Recv(&nb_keys_exp, 1, MPI_INT, NB, KEYEXP, MPI_COMM_WORLD, &status);

	
	//printf("P%2d> Démarré avec la valeur %d\n", rank, value);

	// Si je suis initiateur, initialisation du jeton
	if (initiator) {
		printf("P%2d> Est initiateur (valeur associée %d).\n", rank, value);
		init_table(payload, NB);
		
		// La valeur qu jeton à l'index rank est ma valeur à moi
		payload[rank] = value;
		
		// Identifiant du jeton (pour détruire tous les jetons sauf un)
		payload[token_index] = rank;
		
		// Envoi du jeton à mon voisin de droite
		send_message(right, INIT, payload, payload_size);
	}

	// Tant que le processus est toujours actif...
	while (running) {
		
		/* Réception d'un message.
		   Dans ce contexte, ce sera toujours un message tranportant un jeton. */
		receive_message(&status, payload, payload_size);
		
		if (status.MPI_TAG == INIT) {
			// Message de type "jeton en transit"
			
			/* Supprimer le jeton si :
			   - Je suis initiateur...
			   - et que l'identifiant du jeton est inférieur à mon identifiant */
			if (initiator && (payload[token_index] < rank) ) {
				printf("[INIT] P%2d> Deleted message from %d\n", rank, payload[token_index]);
			} else if (payload[rank] == value) {
				/* Mon jeton m'est retourné, j'envoie un message
				   de type "tous les valeurs de pairs connues" */
				send_message(right, DONE, payload, NB);
				initiateur_gagnant = 1; // pour le détruire quand il me reviendra
			} else {
				/* Sinon, je mets ma valeur à la bonne position,
				   et je laisse passer le message */
				payload[rank] = value;
				send_message(right, INIT, payload, payload_size);
			}
		} else if (status.MPI_TAG == DONE) {
			/* Réception du jeton complet, contenant toute la table
			   des valeurs des pairs. */
			printf("[DONE] P%2d> Received the whole peer_table\n", rank);
			
			// Création d'un tableau de pairs à partir des valeurs
			peer_table = creer_tableau_pair(payload, NB);
			
			/* Classement des valeurs par ordre croissant
			   (indispensable pour créer ma finger table) */
			qsort(peer_table, NB, sizeof(struct pair), qsort_compare_pair);
			
			// Création de la finger table
			struct pair *ftable =
			creer_finger_table(value, nb_keys_exp, peer_table, NB);
			
			/* Libération de la mémoire utilisée par la liste des
			   valeurs de tous les pairs. */
			free(peer_table);
			
			/* Affichage de ma finger table. */
			afficher_finger_table(value, rank, nb_keys_exp, ftable);
			printf("\n\n");
			
			/* Le temps de laisser à MPI d'afficher toutes les lignes,
			   sans entrelacer les lignes des finger table de 
			   plusieurs processus. */
			usleep(50000);
			
			// Je fais passer le message, si je ne dois pas le détruire
			if ( ! initiateur_gagnant) {
				send_message(right, DONE, payload, NB);
			}
			free(payload);
			running = 0;
		}
	}

}

//************   LE PROGRAMME   ***************************
int main(int argc, char *argv[])
{

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &NB);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == --NB) {
		simulator();
	} else {
		peer();
	}
	MPI_Finalize();
	return 0;
}
