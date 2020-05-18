#pragma once

#include "../ex1/utils.h"

struct peer_data {
	// finger table
	struct pair *ftable;
	// finger table size
	int ftsize;
	// reverse finger table
	struct pair *rftable;
	// reverse finger table size
	int rftsize;
};

void send_reverse_messages(int value, struct pair *ftable, int size);
void print_rftable(struct pair *rftable, int size);
struct peer_data *create_peer_data(int nb_peers);
void delete_peer_data(struct peer_data *peer_data);
void print_peer_data(struct peer_data * data);