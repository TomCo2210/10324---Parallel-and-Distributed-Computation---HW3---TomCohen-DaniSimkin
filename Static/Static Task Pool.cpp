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
	int N = 20;
	int x = 0, y;
	double answer = 0;
	double start = 0, end = 0;

	double tempAnswer = 0;
	int rank, numberOfProccesses, taskCount = 0;
	int numOfElements;

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numberOfProccesses);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	numOfElements = (N / (numberOfProccesses - 1));
	start = MPI_Wtime();

	if (rank == 0)
	{
		for (int i = 1; i < numberOfProccesses; i++)
		{
			MPI_Send(&x, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			x += numOfElements;
		}

		for (int i = 1; i < numberOfProccesses; i++)
		{
			MPI_Recv(&tempAnswer, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			answer += tempAnswer;
		}
	}
	else
	{
		MPI_Recv(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		if (N - x - numOfElements < numOfElements)
			numOfElements = N - x;
		for (int i = x; i < x + numOfElements; i++)
			for (y = 0; y < N; y++)
				answer += heavy(i, y);
		MPI_Send(&answer, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}

	end = MPI_Wtime();
	if (rank == 0)
	{
		printf("Time = %f\n", (end - start));
		printf("answer = %e\n", answer);
	}
	MPI_Finalize();
	return 0;
}