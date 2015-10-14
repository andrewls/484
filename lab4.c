#include <mpi.h>
#include <stdio.h>
#include <sys/time.h>


#define ROW_SIZE 16384
#define EPSILON  0.1

float fabs(float f);
double When();

void printGrid(float* ptr) {
  int i = 0;
  for (i = ROW_SIZE; i < (ROW_SIZE + ROW_SIZE * ROW_SIZE); i++) {
    if (i % ROW_SIZE == 0) printf("\n");
    printf("%.2f ", ptr[i]);
  }
  printf("\n");
}

void main(int argc, char *argv[])
{
    float *nA, *oA, *tmp;
    int i, done, reallydone;
    int cnt;
    int start, end;
    int total_cells;

    double starttime;

    int nproc, iproc;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    starttime = When();

    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &iproc);
    // fprintf(stderr,"%d: Hello from %d of %d\n", iproc, iproc, nproc);


    // fprintf(stderr,"Starting initialization for process %d\n", iproc);
    /* Determine how much I should be doing and allocate the arrays*/
    total_cells = ROW_SIZE * ROW_SIZE / nproc;
    nA = (float *)malloc((total_cells + 2 * ROW_SIZE) * sizeof(float));
    oA = (float *)malloc((total_cells + 2 * ROW_SIZE) * sizeof(float));

    start = ROW_SIZE;
    end = total_cells + ROW_SIZE;

    /* Initialize the cells */
    for (i = 0; i < total_cells + 2 * ROW_SIZE; i++)
    {
        nA[i] = oA[i] = 50;
        if (i % ROW_SIZE == 0 || i % ROW_SIZE == ROW_SIZE - 1) nA[i] = oA[i] = 0;
    }

    /* Initialize the Boundaries */
    if (iproc == 0)
    {
        start += ROW_SIZE;
        for (i = 0; i < ROW_SIZE; i++) {
          nA[ROW_SIZE + i] = oA[ROW_SIZE + i] = 100;
        }
        // nA[1] = oA[1] = 100;
    }
    if (iproc == nproc - 1)
    {
        end = total_cells;
        for (i = 0; i < ROW_SIZE; i++) {
          nA[total_cells + i] = oA[total_cells + i] = 0;
        }
        // nA[total_cells] = oA[total_cells] = 0;
    }

	// fprintf(stderr,"Done initializing\n");

    /* Now run the relaxation */
    reallydone = 0;
    for(cnt = 0; !reallydone; cnt++)
    {
        /* First, I must get my neighbors boundary values */
        if (iproc != 0)
        {
            if (iproc % 2 != 0) MPI_Recv(&oA[0], ROW_SIZE, MPI_FLOAT, iproc - 1, 0, MPI_COMM_WORLD, &status);
            MPI_Send(&oA[ROW_SIZE], ROW_SIZE, MPI_FLOAT, iproc - 1, 0, MPI_COMM_WORLD);
            if (iproc % 2 == 0) MPI_Recv(&oA[0], ROW_SIZE, MPI_FLOAT, iproc - 1, 0, MPI_COMM_WORLD, &status);
        }
        else {
          fprintf(stderr,"Iteration %d\n", cnt);
        }
	      if (iproc != nproc - 1)
        {
            if (iproc % 2 != 0) MPI_Recv(&oA[total_cells + ROW_SIZE], ROW_SIZE, MPI_FLOAT, iproc + 1, 0, MPI_COMM_WORLD, &status);
            MPI_Send(&oA[total_cells], ROW_SIZE, MPI_FLOAT, iproc + 1, 0, MPI_COMM_WORLD);
            if (iproc % 2 == 0) MPI_Recv(&oA[total_cells + ROW_SIZE], ROW_SIZE, MPI_FLOAT, iproc + 1, 0, MPI_COMM_WORLD, &status);
        }


        /* Do the calculations */
        for (i = start; i < end; i++)
        {
            if (i % ROW_SIZE == 0 || i % ROW_SIZE == ROW_SIZE - 1) continue;
            nA[i] = (oA[i] * 4 + oA[i-1] + oA[i + 1] + oA[i-ROW_SIZE] + oA[i + ROW_SIZE]) / 8;
        }

        // printGrid(nA);
        /* Check to see if we are done */
        done = 1;
        for (i = start; i < end; i++)
        {
            if (i % ROW_SIZE == 0 || i % ROW_SIZE == ROW_SIZE - 1) continue;
            if (fabs(nA[i] - (nA[i - 1] + nA[i + 1] + nA[i - ROW_SIZE] + nA[i + ROW_SIZE])/4) > EPSILON)
            {
                done = 0;
                break;
            }
        }
        /* Do a reduce to see if everybody is done */
        MPI_Allreduce(&done, &reallydone, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);

        /* Swap the pointers */
        tmp = nA;
        nA = oA;
        oA = tmp;
    }

    /* print out the number of iterations to relax */
    fprintf(stderr, "%d:It took %d iterations and %lf seconds to relax the system\n",
                                   iproc,cnt,When() - starttime);
    MPI_Finalize();
}

float fabs(float f)
{
    return (f > 0.0)? f : -f ;
}

/* Return the correct time in seconds, using a double precision number.       */
double
When()
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return ((double) tp.tv_sec + (double) tp.tv_usec * 1e-6);
}
