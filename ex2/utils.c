#include <stdio.h>
#include "utils.h"

/**
 * Receive an mpi INT message from any source.
 */
void receive_message(MPI_Status *status, int *value, int size)
{
	MPI_Recv(value, size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, status);
}

/**
 * Send an mpi INT message.
 */
void send_message(int dest, int tag, int *val, int size)
{
	// printf("P%d> Send message %d to %d\n", rank, tag, dest);
	MPI_Send(val, size, MPI_INT, dest, tag, MPI_COMM_WORLD);
}

/**
 * Init a table with EMPTY value.
 * (initialisation d'un jeton vide)
 */
void init_table(int *table, int size)
{
	for (int i = 0; i < size; i++) {
		table[i] = EMPTY;
	}
}

void print_table(int *table, int size)
{
	printf("Peer table:\n");
	for (int i = 0; i < size; i++) {
		printf("%2d: %5d\n", i, table[i]);
	}
}

/**
 * Quickly display a finger table.
 */
void print_ftable(struct pair *table, int size)
{
	printf("Finger table:\n");
	for (int i = 0; i < size; i++) {
		printf("%2d: %5d\n", i, table[i].valeur);
	}
}
