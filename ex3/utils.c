#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "tags.h"
#include "utils.h"
#include"../ex2/utils.h"

/**
 * Send a REVERSE message to every concerned peer.
 */
void send_reverse_messages(int value, struct pair *ftable, int size)
{
	int prev = EMPTY;
	for (int i = 0; i < size; i++) {
		if (ftable[i].valeur != prev) {
			send_message(ftable[i].rang, REVERSE, &value, 1);
			prev = ftable[i].valeur;
		}
	}
}

/**
 * Print the reverse table.
 */
void print_rftable(struct pair *rftable, int size)
{
	printf("Reverse ");
	print_ftable(rftable, size);
}

struct peer_data *create_peer_data(int nb_peers)
{
	struct peer_data *peer_data = malloc(sizeof(struct peer_data));
	peer_data->rftable = malloc(sizeof(struct pair) * nb_peers);
	peer_data->rftsize = 0;
	return peer_data;
}

void delete_peer_data(struct peer_data *peer_data)
{
	free(peer_data->ftable);
	free(peer_data->rftable);
	free(peer_data);
}

void print_peer_data(struct peer_data *data)
{
	print_ftable(data->ftable, data->ftsize);
	print_rftable(data->rftable, data->rftsize);
}