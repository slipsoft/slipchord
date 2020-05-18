#include <stdio.h>
#include "utils.h"

/**
 * Init a table with EMPTY value.
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
