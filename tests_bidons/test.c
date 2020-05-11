
#include <stdio.h>
#include <stdlib.h> // pour rand et srand
#include <time.h>   // pour time



int main(int argc, char *argv[])
{
	printf("Correctement lancé.\n");
	
	for (int i = 0; i < 3; ++i) {
		printf("i %d  pow %d\n", i, 1 << i);
	}
}