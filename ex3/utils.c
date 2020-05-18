#include <stdio.h>
#include <mpi.h>
#include "tags.h"
#include "utils.h"
#include"../ex2/utils.h"

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

void print_rftable(struct pair *rftable, int size)
{
	printf("Reverse ");
	print_ftable(rftable, size);
}