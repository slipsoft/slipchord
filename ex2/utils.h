#pragma once

#include <mpi.h>
#include "../ex1/init_valeurs_pairs.h"

#define EMPTY -1 // Symbolyses an empty value in the peer_table.

void receive_message(MPI_Status *status, int *value, int size);
void send_message(int dest, int tag, int *val, int size);
void init_table(int *table, int size);
void print_table(int *table, int size);
void print_ftable(struct pair *table, int size);
