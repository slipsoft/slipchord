
#include <stdio.h>
#include <stdlib.h> // pour rand et srand
#include <time.h>   // pour time

int appartient(int a, int b, int k, int N);
int a_moins_b(int a, int b, int N);
int b_moins_a(int a, int b, int N);
int a_inf_b(int a, int b, int k);
int appartient_arc_oriente_large(int a, int b, int k, int N);
int appartient_arc_oriente_strict(int a, int b, int k, int N);


/*int appartient(int a, int b, int k, int N)
{
	if (a)
}*/

int appartient_arc_oriente_large(int a, int b, int k, int N)
{
	if (a == b) return 0;
	if (k == a) return 1;
	if (k == b) return 1;
	return appartient_arc_oriente_strict(a, b, k, N);
}

int appartient_arc_oriente_strict(int a, int b, int k, int N)
{
	k = k % N;
	// Cas simple, comme dans les entiers naturels
	if (a <= b) {
		if ( (a < k) && (k < b) ) return 1;
		return 0;
	} else {
		int entre_a_et_zero = (k > a); // k <= N de base
		int entre_b_et_zero = (k < b); // k >= 0 de base
		if (entre_a_et_zero || entre_b_et_zero) return 1;
		return 0;
	}
}


int appartient_arc_oriente_ancien(int a, int b, int k, int N)
{
	int c1 = a_moins_b(k, a, N);
	int c2 = a_moins_b(k, b, N);
	printf("%d ? [%d,%d] ->   (k-a) %d   (k-b)   %d \n", k, a, b, c1, c2);
	
	if ( (c1 >= 0) && (c2 <= 0) ) return 1;
	return 0;
	
}

/*void test_appartient(int a, int b, int k)
{
	int res = appartient_arc_oriente(a, b, k, 12);
	printf("%d dans [%d, %d]   :   %d \n", k, a, b, res);
	printf("--------------------------\n");
}*/

void test_app2(int a, int b, int k)
{
	
	int sup2prev = a_inf_b(a-1, k, 64);
	int inf2curr = a_inf_b(k, b+1, 64);
	
	int cn = (sup2prev && inf2curr);
	
	int res = appartient_arc_oriente_large(a, b, k, 64);
	printf("%d dans [%d, %d]   :   %d   %d \n", k, a, b, res, cn);
	printf("--------------------------\n");
}

int a_moins_b(int a, int b, int N) {
	return - b_moins_a(a, b, N);
}

// retourne le résultat de la soustraction entre a et b (cyclique)
int b_moins_a(int a, int b, int N)
{
	// soustraction sur les entiers positive
	if ( (b - a) > 0 ) {
		// résultat >= 0 : b plus grand au sens cyclique
		if ( (b - a) < N / 2 ) return b - a;
		// résultat <= 0 : a plus grand au sens cyclique
		return (b - a) - N; // (distance bien indéfieure à k/2 car ((b-a) >= k/2))
	} else /* (b - a) <= 0 */ {
		// résultat <= 0 : a plus grand au sens cyclique
		if ( (a - b) < N / 2 ) return b - a;
		// résultat >= 0 : b plus grand au sens cyclique
		return N + (b - a);
	}
}

int a_inf_b(int a, int b, int k) {
	int res = b_moins_a(a, b, k);
	
	if (res > 0) return 1;
	return 0;
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
	/*test_appartient(1, 3, 4);
	test_appartient(1, 3, 2);
	test_appartient(1, 9, 4);
	test_appartient(9, 2, 1);
	test_appartient(9, 0, 0);*/
	test_app2(38, 42, 7);
	test_app2(38, 48, 7);
	test_app2(38, 8, 7);
	
	/*test_heure(9, 0);
	test_heure(9, 12);
	test_heure(0, 9);
	test_heure(0, 6);
	test_heure(6, 0);
	
	test_heure(10, 4);
	test_heure(10, 5);
	test_heure(5, 10);*/
	
	
	
	/*
	for (int i = 0; i < 3; ++i) {
		printf("i %d  pow %d\n", i, 1 << i);
	}*/
}