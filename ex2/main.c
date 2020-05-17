#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include "utils.h"

//************   TAGS
#define INIT          0 // Message to init the peertable
#define DONE          1 // Message to propagate the peertable

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

//************   LES FONCTIONS   ***************************

void receive_message(MPI_Status *status, int *value)
{
	MPI_Recv(value, NB, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, status);
}

void send_message(int dest, int tag, int *val)
{
	// printf("P%d> Send message %d to %d\n", rank, tag, dest);
	MPI_Send(val, NB, MPI_INT, dest, tag, MPI_COMM_WORLD);
}

int check_termination()
{
	return running;
}

//************   LE PROGRAMME   ***************************
int main(int argc, char *argv[])
{

	MPI_Status status;
	int *payload = malloc(sizeof(int) * NB + 1);
	int token_index = NB;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &NB);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int *payload = malloc(sizeof(int) * NB + 1);
	int token_index = NB;

	srand(getpid());

	left = (rank + NB - 1) % NB;
	right = (rank + 1) % NB;
	value = rand() % (1 << NB);
	initiator = rand() % 2;

	printf("P%d> Started with value %d\n", rank, value);


	if (initiator) {
		printf("P%d> Is initiator\n", rank);
		init_table(payload, NB);
		payload[rank] = value;
		payload[token_index] = rank;
		send_message(right, INIT, payload);
	}

	while (check_termination()) {
		receive_message(&status, payload);
		if (status.MPI_TAG == INIT) {
			// peer_table initialization message.
			if (initiator && payload[token_index] < rank) {
				printf("P%d> Deleted one INIT message\n", rank);
			} else if (payload[rank] == value) {
				send_message(right, DONE, payload);
			} else {
				payload[rank] = value;
				send_message(right, INIT, payload);
			}
		} else if (status.MPI_TAG == DONE) {
			// the peer_table is initialized.
			printf("P%d> Received the whole peer_table\n", rank);
			print_table(payload, NB);
			send_message(right, DONE, payload);
			running = 0;
		}
	}

	free(payload);
	MPI_Finalize();
	return 0;
}
