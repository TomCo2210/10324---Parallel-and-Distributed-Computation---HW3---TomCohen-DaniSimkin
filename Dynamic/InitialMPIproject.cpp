// InitialMPIproject.cpp : Defines the entry point for the console application.
//

/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
*  (C) 2001 by Argonne National Laboratory.
*      See COPYRIGHT in top-level directory.
*/

/* This is an interactive version of cpi */
#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[])
{

	int  namelen, numprocs, myid;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	MPI_Get_processor_name(processor_name, &namelen);

	MPI_Status status;
	int x = 7, y = 10, answer = 77777;
	if (myid == 0) {
		MPI_Send(&x, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Recv(&answer, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
	}
	else {
		MPI_Recv(&y, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		y = y * 3;
		MPI_Send(&y, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	//if (myid == 0)
	printf("answer = %d numprocs = %d  myid = %d   %s\n", answer, numprocs, myid, processor_name);

	MPI_Finalize();
	return 0;
}

