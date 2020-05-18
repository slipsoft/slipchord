#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include "utils.h"
#include "../ex1/init_valeurs_pairs.h"

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
// Left and right neighbours
int left, right;
int running = 1;
int initiator;

/**
 * Process of the simulator.
 */
void simulator()
{
	int *peertable;
	int nb_peers = NB;
	int nb_keys_exp = 6;
	init_pairs_aleatoire_non_classe(&peertable, nb_peers, nb_keys_exp);
	for (int i = 0; i < nb_peers; i++) {
		send_message(i, VALUE, &peertable[i], 1);
		send_message(i, KEYEXP, &nb_keys_exp, 1);
	}
	free(peertable);
}

/**
 * Process of a classic chord peer.
 */
void peer()
{
	MPI_Status status;
	int payload_size = NB + 1;
	int *payload = malloc(sizeof(int) * payload_size);
	int token_index = NB;
	struct pair *peer_table;

	srand(getpid());

	left = (rank + NB - 1) % NB;
	right = (rank + 1) % NB;
	initiator = rand() % 2;
	MPI_Recv(&value, 1, MPI_INT, NB, VALUE, MPI_COMM_WORLD, &status);
	MPI_Recv(&nb_keys_exp, 1, MPI_INT, NB, KEYEXP, MPI_COMM_WORLD, &status);

	printf("P%d> Started with value %d\n", rank, value);


	if (initiator) {
		printf("P%d> Is initiator\n", rank);
		init_table(payload, NB);
		payload[rank] = value;
		payload[token_index] = rank;
		send_message(right, INIT, payload, payload_size);
	}

	while (running) {
		receive_message(&status, payload, payload_size);
		if (status.MPI_TAG == INIT) {
			// peer_table initialization message.
			if (initiator && payload[token_index] < rank) {
				printf("P%d> Deleted message from %d\n", rank, payload[token_index]);
			} else if (payload[rank] == value) {
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
			struct pair *ftable = creer_finger_table(
				value,
				nb_keys_exp,
				peer_table,
				NB
			);
			free(peer_table);
			printf("P%d> value %d\n", rank, value);
			print_ftable(ftable, nb_keys_exp);
			send_message(right, DONE, payload, NB);
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
