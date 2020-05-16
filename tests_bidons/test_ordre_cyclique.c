
#include <stdio.h>
#include <stdlib.h> // pour rand et srand
#include <time.h>   // pour time

// retourne le résultat de la soustraction entre a et b (cyclique)
int b_moins_a(int a, int b, int k)
{
	// soustraction sur les entiers positive
	if ( (b - a) > 0 ) {
		// résultat >= 0 : b plus grand au sens cyclique
		if ( (b - a) < k / 2 ) return b - a;
		// résultat <= 0 : a plus grand au sens cyclique
		return (b - a) - k; // (distance bien indéfieure à k/2 car ((b-a) >= k/2))
	} else /* (b - a) <= 0 */ {
		// résultat <= 0 : a plus grand au sens cyclique
		if ( (a - b) < k / 2 ) return b - a;
		// résultat >= 0 : b plus grand au sens cyclique
		return k + (b - a);
	}
}

void test_heure(int a, int b)
{
	int k = 12; // 12h sur le cadran
	int res = b_moins_a(a, b, k);
	//printf("a(%d) b(%d)   t(%d) \n", a, b, res);
	printf("%d -> %d   =   %d \n", a, b, res);
	printf("--------------------------\n");
}

int main(void) // int argc, char *argv[]
{
	printf("Correctement lancé.\n");
	
	// Test sur les heures de la journée (c'est simple et visuel)
	
	printf("--------------------------\n");
	test_heure(9, 0);
	test_heure(9, 12);
	test_heure(0, 9);
	test_heure(0, 6);
	test_heure(6, 0);
	
	test_heure(10, 4);
	test_heure(10, 5);
	test_heure(5, 10);
	
	
	
	/*
	for (int i = 0; i < 3; ++i) {
		printf("i %d  pow %d\n", i, 1 << i);
	}*/
}