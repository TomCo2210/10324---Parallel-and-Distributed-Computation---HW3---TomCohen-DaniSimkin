#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define HEAVY 100000
#define DO_TAG 0
#define DONE_TAG 1
#define RES_TAG 2

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
void singleJobSender(int* tasksDone, int* taskCount, int proccess)
{
	MPI_Send(tasksDone, 1, MPI_INT, proccess, DO_TAG, MPI_COMM_WORLD);
	(*tasksDone)++;
	(*taskCount)++;
}

void jobReciever(int* tasksDone, MPI_Status* status, double* answer, int N)
{
	MPI_Recv(tasksDone, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, status);
	while (status->MPI_TAG == DO_TAG) {
		*answer = 0;
		for (int y = 0; y < N; y++)
			*answer += heavy(*tasksDone, y);
		MPI_Send(answer, 1, MPI_DOUBLE, 0, RES_TAG, MPI_COMM_WORLD);
		MPI_Recv(tasksDone, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, status);
	}
}

int main(int argc, char *argv[]) {
	int N = 20;
	int numberOfSlaves = 0, tasksDone = 0;
	int rank, numberOfProccesses, taskCount = 0;
	double answer = 0, tempAnswer = 0, start = 0, end = 0;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numberOfProccesses);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	start = MPI_Wtime();
	if (rank == 0)
	{
		for (int i = 1; i < numberOfProccesses; i++)
			singleJobSender(&tasksDone, &taskCount, i);
		do {
			MPI_Recv(&tempAnswer, 1, MPI_DOUBLE, MPI_ANY_SOURCE, RES_TAG, MPI_COMM_WORLD, &status);
			taskCount--;
			if (tasksDone < N)
				singleJobSender(&tasksDone, &taskCount, status.MPI_SOURCE);
			else
				MPI_Send(&tasksDone, 1, MPI_INT, status.MPI_SOURCE, DONE_TAG, MPI_COMM_WORLD);
			answer += tempAnswer;
		} while (taskCount > 0);
	}
	else
		jobReciever(&tasksDone, &status, &tempAnswer, N);

	if (rank == 0)
	{
		end = MPI_Wtime();
		printf("Time = %f, answer = %e\n", (end - start), answer);
	}
	MPI_Finalize();
	return 0;
}