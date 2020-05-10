#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
	int my_rank;
	int nb_proc;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

	printf("Processus %d sur %d : Hello\n", my_rank, nb_proc);

	MPI_Finalize();
	return 0;
}