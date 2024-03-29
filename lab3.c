#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

//#define VECSIZE 65536
#define VECSIZE 1
//#define ITERATIONS 10000
#define ITERATIONS 10000

typedef struct nodeElement {
  double val;
  int rank;
} NodeElement;

// Broadcast vector to all nodes
void broadcast(int numdim, int rank, NodeElement * vector, int vectorSize, MPI_Datatype datatype) {
  MPI_Status status;
  int notparticipating = pow(2,numdim-1)-1;
  int bitmask = pow(2,numdim-1);
  int curdim = 0;
  for(curdim = 0; curdim < numdim; curdim++) {
    if ((rank & notparticipating) == 0) {
       if ((rank & bitmask) == 0) {
	         int msg_dest = rank ^ bitmask;
           MPI_Send(vector, vectorSize, datatype, msg_dest, 0, MPI_COMM_WORLD);
        }
        else {
          int msg_src = rank ^ bitmask;
          MPI_Recv(vector, vectorSize, datatype, msg_src, 0, MPI_COMM_WORLD, &status);
        }
    }
    notparticipating >>= 1;
    bitmask >>=1;
  }
}

// Reduce values to one node
void maxReduce(int numdim, int rank, NodeElement * vector, int vectorSize, MPI_Datatype datatype)
{
  MPI_Status status;
  int notparticipating = 0;
  int curdim = 0;
  int bitmask = 1;
  // allocate a new array to hold received values
  NodeElement * newValues = malloc(vectorSize * sizeof(NodeElement));
  for(curdim = 0; curdim < numdim; curdim++) {
     if ((rank & notparticipating) == 0) {
       if ((rank & bitmask) != 0) {
         int msg_dest = rank ^ bitmask;
         MPI_Send(vector, vectorSize, datatype, msg_dest, 0, MPI_COMM_WORLD);
       }
       else {
         int msg_src = rank ^ bitmask;
         MPI_Recv(newValues, vectorSize, datatype, msg_src, 0, MPI_COMM_WORLD, &status);
         int i;
         for (i = 0; i < vectorSize; i++) {
           if ((*(newValues + i)).val > (*(vector + i)).val) {
             *(vector + i) = *(newValues + i);
           }
         }
       }
     }
     notparticipating = notparticipating ^ bitmask;
     bitmask <<=1;
  }
}


double When() {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return ((double) tp.tv_sec + (double) tp.tv_usec * 1e-6);
}

main(int argc, char *argv[]) {
        int iproc, nproc,i, iter;
        char host[255], message[55];
        MPI_Status status;

        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &nproc);
        MPI_Comm_rank(MPI_COMM_WORLD, &iproc);

        gethostname(host,253);
        // printf("I am proc %d of %d running on %s\n", iproc, nproc,host);
        // each process has an array of VECSIZE double: ain[VECSIZE]
        double ain[VECSIZE], aout[VECSIZE];
        int  ind[VECSIZE];
        NodeElement in[VECSIZE], out[VECSIZE];
        int myrank, root = 0;

        MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
        // Start time here
        srand(myrank+5);
        double start = When();
        for(iter = 0; iter < ITERATIONS; iter++) {
          for(i = 0; i < VECSIZE; i++) {
            ain[i] = rand();
          //printf("init proc %d [%d]=%f\n",myrank,i,ain[i]);
          }
          for (i=0; i<VECSIZE; ++i) {
            in[i].val = ain[i];
            in[i].rank = myrank;
          }
          maxReduce((int)log2(nproc), myrank, in, VECSIZE, MPI_DOUBLE_INT);
	  //printf("starting max reduce.\n");
	  memcpy(out, in, sizeof(NodeElement) * VECSIZE);
          // MPI_Reduce( in, out, VECSIZE, MPI_DOUBLE_INT, MPI_MAXLOC, root, MPI_COMM_WORLD);
          // At this point, the answer resides on process root
          if (myrank == root) {
              /* read ranks out
               */
              for (i=0; i<VECSIZE; ++i) {
                //printf("root out[%d] = %f from %d\n",i,out[i].val,out[i].rank);
                  aout[i] = out[i].val;
                  ind[i] = out[i].rank;
              }
          }
          // Now broadcast this max vector to everyone else.
	  //printf("Starting broadcast.");
          broadcast((int) log2(nproc), myrank, in, VECSIZE, MPI_DOUBLE_INT);
	  memcpy(out, in, sizeof(NodeElement) * VECSIZE);
          // MPI_Bcast(out, VECSIZE, MPI_DOUBLE_INT, root, MPI_COMM_WORLD);
          for(i = 0; i < VECSIZE; i++) {
          //printf("final proc %d [%d]=%f from %d\n",myrank,i,out[i].val,out[i].rank);
          }
        }
        MPI_Finalize();
        double end = When();
        if(myrank == root) {
          printf("Time %f\n",end-start);
        }
}
