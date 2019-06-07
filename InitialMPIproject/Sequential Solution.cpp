#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define HEAVY 100000

// This function performs heavy computations, 
// its run time depends on x and y values

double heavy(int x, int y) {
	int i, loop = 1;
	double sum = 0;

	// Super heavy tasks
	if (x < 5 || y < 5)
		loop = 10;
	// Heavy calculations
	for (i = 0; i < loop*HEAVY; i++)
		sum += sin(exp(sin((double)i / HEAVY)));

	return sum;
}


int main(int argc, char *argv[]) {
	int x, y;
	int N = 20;
	double answer = 0;
	double start = 0, end = 0;

	MPI_Init(&argc, &argv);
	start = MPI_Wtime();
	for (x = 0; x < N; x++)
		for (y = 0; y < N; y++)
			answer += heavy(x, y);

	printf("answer = %e\n", answer);
	end = MPI_Wtime();
	printf("Time = %f, answer = %e\n", (end - start), answer);
	MPI_Finalize();
}
