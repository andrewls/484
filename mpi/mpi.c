#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


main(int argc, char *argv[])
{
	int iproc, nproc,i, taskid, destid, srcid, recvval;
	char host[255], message[55];
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
	destid = taskid + 1;
	if (destid > nproc) destid = 0;
	printf("MPI task %d of %d sending to %d\n", taskid, nproc, destid);
	MPI_Send(&taskid, 1, MPI_INT, destid, 0, MPI_COMM_WORLD);
	srcid = taskid - 1;
	if (srcid < 0) srcid = nproc - 1;
	MPI_Recv(&recvval, 1, MPI_INT, srcid, 0, MPI_COMM_WORLD, &status);
	printf("MPI task %d of %d got %d\n", taskid, nproc, recvval);
	MPI_Finalize();
}
