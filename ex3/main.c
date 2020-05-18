#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include "tags.h"
#include "../ex1/utils.h"
#include "../ex2/utils.h"
#include "../ex3/utils.h"

#define NB_KEY_EXP 6 // nb power of 2 keys

//************   VARIABLES MPI
// total process number
int NB;
// Current process mpi id
int rank;
// Current Process chord value
int value;
// Left and right neighbours
int left, right;
int running = 1;
int initiator;

/**
 * Process of the simulator.
 */
void simulator()
{
	MPI_Status status;
	int *peertable;
	int nb_peers = NB;
	int i = 0;
	init_pairs_aleatoire_non_classe(&peertable, nb_peers, NB_KEY_EXP);
	for (i = 0; i < nb_peers - 1; i++) {
		send_message(i, VALUE, &peertable[i], 1);
	}
	MPI_Recv(&value, 1, MPI_INT, MPI_ANY_SOURCE, INIT_END, MPI_COMM_WORLD, &status);
	send_message(i, VALUE, &peertable[i], 1);
	free(peertable);
}

void newpeer()
{
	MPI_Status status;
	int payload_size = NB + 1;
	int *payload = malloc(sizeof(int) * payload_size);
	struct peer_data *data = create_peer_data(NB);

	MPI_Recv(&value, 1, MPI_INT, MPI_ANY_SOURCE, VALUE, MPI_COMM_WORLD, &status);
	data->ftsize = NB_KEY_EXP;

	int chord_next;
	int rank_prev = rank - 1;
	payload[0] = value + 1;

	printf("P%d> Started with value %d payload_size = %d\n", rank, value, payload_size);
	MPI_Send(payload, payload_size, MPI_INT, rank_prev, TAG_RECHERCHE_INIT, MPI_COMM_WORLD);
	MPI_Recv(&chord_next, 1, MPI_INT, rank_prev, TAG_RECHERCHE_TROUVE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	printf("FIND NEXT CHORD PEER: %d", chord_next);

	delete_peer_data(data);
}

/**
 * Process of a classic chord peer.
 */
void peer()
{
	MPI_Status status;
	int winner = 0;
	int payload_size = NB + 1;
	int *payload = malloc(sizeof(int) * payload_size);
	int token_index = NB;
	struct pair *peer_table;
	struct peer_data *data = create_peer_data(NB);

	int valeur_clef;
	// à qui renvoyer le message lorsque le pair responsable est trouvé
	int rang_retour_message;
	int doit_renvoyer_message;
	// Pour factoriser les fonctions, vrai si je dois rechercher un pair
	int doit_rechercher_clef = 0;


	srand(getpid());

	left = (rank + NB - 1) % NB;
	right = (rank + 1) % NB;
	initiator = rand() % 2;
	MPI_Recv(&value, 1, MPI_INT, MPI_ANY_SOURCE, VALUE, MPI_COMM_WORLD, &status);
	data->ftsize = NB_KEY_EXP;

	printf("P%d> Started with value %d payload_size = %d\n", rank, value, payload_size);


	if (initiator) {
		printf("P%d> Is initiator\n", rank);
		init_table(payload, NB);
		payload[rank] = value;
		payload[token_index] = rank;
		send_message(right, INIT, payload, payload_size);
	}

	while (running) {
		receive_message(&status, payload, payload_size);
		valeur_clef = payload[0];
		if (status.MPI_TAG == INIT) {
			// peer_table initialization message.
			if (initiator && payload[token_index] < rank) {
				printf("P%d> Deleted message from %d\n", rank, payload[token_index]);
			} else if (payload[rank] == value) {
				winner = 1;
				send_message(right, DONE, payload, NB);
			} else {
				payload[rank] = value;
				send_message(right, INIT, payload, payload_size);
			}
		} else if (status.MPI_TAG == DONE) {
			// the peer_table is initialized.
			printf("P%d> Received the whole peer_table\n", rank);
			peer_table = creer_tableau_pair(payload, NB);
			// sort the peer table by the chrod value
			qsort(peer_table, NB, sizeof(struct pair), qsort_compare_pair);
			data->ftable = creer_finger_table(
				value,
				data->ftsize,
				peer_table,
				NB
			);
			free(peer_table);
			send_reverse_messages(value, data->ftable, data->ftsize);
			if (!winner) {
				send_message(right, DONE, payload, NB);
			} else {
				send_message(NB + 1, INIT_END, payload, 1);
				send_message(right, PRINT, payload, 1);
			}
		} else if (status.MPI_TAG == REVERSE) {
			data->rftable[data->rftsize].rang = status.MPI_SOURCE;
			data->rftable[data->rftsize].valeur = payload[0];
			data->rftsize++;
		} else if (status.MPI_TAG == PRINT) {
			printf("P%d> value %d\n", rank, value);
			print_peer_data(data);
			if (!winner) {
				send_message(right, PRINT, payload, 1);
			} else {
				send_message(right, END, payload, 1);
			}
		// } else if (status.MPI_TAG == END) {
		// 	running = 0;
		// 	if (!winner) {
		// 		send_message(right, END, payload, 1);
		// 	}
		} else if (status.MPI_TAG == TAG_RECHERCHE_INIT) {
			// Le processus init me demande de rechercher une clef
			doit_rechercher_clef = 1;
			doit_renvoyer_message = 0;
			rang_retour_message = status.MPI_SOURCE; // le message devra m'être retourné// Je recherche le processus à qui envoyer ma demande
			printf("(pair %d rg %d) : Simulateur me demande de rechercher la clef %d\n\n", value, rank, valeur_clef);
		} else if (status.MPI_TAG == TAG_RECHERCHE_PAIR) {
			// Un pair me demande de rechercher une clef
			doit_rechercher_clef = 1;
			doit_renvoyer_message = 1;
			// Je reçois le rang duquel je vais devenoir recevoir un message
			MPI_Recv(&rang_retour_message, 1, MPI_INT, status.MPI_SOURCE, TAG_RECHERCHE_PAIR, MPI_COMM_WORLD, &status);
		}
		
		
		if (doit_rechercher_clef) {
			int resp_index = trouver_index_responsable(data->ftable,
				NB_KEY_EXP, valeur_clef, value);
			
			int resp_valeur;
			int resp_rang;
			
			if (resp_index == -2) {
				// Je suis responsable de la clef
				resp_valeur = value;
				resp_rang = rank;
				printf("(pair %d rg %d) : je suis responsable de la clef(%d).\n\n", value, rank, valeur_clef);

				
			} else if (resp_index == -1) {
				// Mon successeur est responsable de la clef
				resp_valeur = data->ftable[0].valeur;
				resp_rang = data->ftable[0].rang;
				printf("(pair %d rg %d) : mon successeur(%d) est responsable de la clef(%d).\n\n",
					value, rank, resp_valeur, valeur_clef);
				
			} else {
				doit_renvoyer_message = 0; // ce n'est pas à moi de renvoyer le message, comme je passe le message.
				// Il faut que j'envoie une demande à un pair pour trouver le responsable
				printf("(pair %d rg %d) : j'envoie la demande au pair %d (rg %d) (recherche de la clef %d).\n\n",
					value, rank, data->ftable[resp_index].valeur, data->ftable[resp_index].rang, valeur_clef);
				
				// Envoi de la demande au pair
				int dest = data->ftable[resp_index].rang;
				MPI_Send(&valeur_clef, 1, MPI_INT, dest, TAG_RECHERCHE_PAIR, MPI_COMM_WORLD);
				
				// Envoi du rang à qui renvoyer le message, une fois le responsable trouvé,
				// pour que le message puisse revenir directement au pair qui a fait la permière demande.
				MPI_Send(&rang_retour_message, 1, MPI_INT, dest, TAG_RECHERCHE_PAIR, MPI_COMM_WORLD);
				
				// Si je suis le processus qui a fait la demande initiale, j'attends la réponse
				// if (rang_retour_message == rank)
				if (status.MPI_TAG == TAG_RECHERCHE_INIT) {
					// Réception de la réponse (du pair dont le successeur est responsable de la clef)
					MPI_Recv(&resp_valeur, 1, MPI_INT, MPI_ANY_SOURCE, TAG_RECHERCHE_TROUVE, MPI_COMM_WORLD, &status);
					MPI_Recv(&resp_rang, 1, MPI_INT, status.MPI_SOURCE, TAG_RECHERCHE_TROUVE, MPI_COMM_WORLD, &status);
					printf("(pair %d rg %d) : réception (de rg %d) du pair responsable de la clef : valeur(%d) rang_MPI(%d).\n\n",
						value, rank, status.MPI_SOURCE, resp_valeur, resp_rang);
				}
			}
			
			
			
			// Si je suis le processus qui a fait la demande initiale,
			// ma demande a été bloquante, du coup là j'envoie un message au processus
			// init.
			//if (status.MPI_TAG == TAG_RECHERCHE_INIT) {
			if (doit_renvoyer_message) {
				MPI_Send(&resp_valeur, 1, MPI_INT, rang_retour_message, TAG_RECHERCHE_TROUVE, MPI_COMM_WORLD);
				MPI_Send(&resp_rang, 1, MPI_INT, rang_retour_message, TAG_RECHERCHE_TROUVE, MPI_COMM_WORLD);
			} else {
				// i.e. (status.MPI_TAG == TAG_RECHERCHE_INIT)
				// Envoi au simulateur (le simulateur va broadcast un message de terminaison)
				MPI_Send(&resp_valeur, 1, MPI_INT, rang_retour_message, TAG_RECHERCHE_TROUVE, MPI_COMM_WORLD);
			}
			
		}
		
	}
	delete_peer_data(data);
	free(payload);

}

//************   LE PROGRAMME   ***************************
int main(int argc, char *argv[])
{

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &NB);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == --NB) {
		simulator();
	} else if (rank == --NB) {
		newpeer();
	} else {
		peer();
	}
	MPI_Finalize();
	return 0;
}
