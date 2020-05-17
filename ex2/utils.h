#pragma once

#include "../ex1/init_valeurs_pairs.h"

#define EMPTY -1 // Symbolyses an empty value in the peer_table.

void init_table(int *table, int size);
void print_table(int *table, int size);
void print_ftable(struct ftable_element *table, int size);
