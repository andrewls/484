#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define GRID_SIZE 16384
#define NUM_THREADS 1
#define ROWS_PER_THREAD GRID_SIZE/NUM_THREADS
#define THREAD_LAST_ROW(i) i + ROWS_PER_THREAD


typedef struct linear_barrier_t {
	int num_threads_to_wait_for;
	int num_threads_passed_through;
	pthread_mutex_t * mutex;
} linear_barrier_t;

void linear_barrier_init(linear_barrier_t * barrier) {
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);
	barrier->num_threads_to_wait_for = NUM_THREADS;
	barrier->num_threads_passed_through = 0;
	barrier->mutex = &mutex;
}

void linear_barrier_wait(linear_barrier_t * barrier) {
	pthread_mutex_lock(barrier->mutex);
	barrier->num_threads_passed_through++;
	pthread_mutex_unlock(barrier->mutex);
	while(barrier->num_threads_passed_through != barrier->num_threads_to_wait_for);
	pthread_mutex_lock(barrier->mutex);
	barrier->num_threads_passed_through--;
	pthread_mutex_unlock(barrier->mutex);
}

// these three are the grids used by the main function
float *temp, *grid, *oldGrid;
linear_barrier_t barrier;

void print_grid(float* grid) {
	int i, j;
	for (i = 0; i < GRID_SIZE; i++) {
		printf("[");
		for (j=0; j < GRID_SIZE; j++) {
			printf("%.2f", grid[i*GRID_SIZE + j]);
			if (j != GRID_SIZE - 1) printf(", ");
		}
		printf("]\n");
	}
	printf("\n");
}


double when() {
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return ((double) tp.tv_sec + (double) tp.tv_usec * 1e-6);
}

float abs_val(float num) {
	if (num > 0) return num;
	return num * -1;
}

int steady(float* grid) {
	int i, j;
	for (i = 1; i < GRID_SIZE - 1; i++) {
		for (j= 1; j < GRID_SIZE - 1; j++) {
			if (abs_val(grid[i * GRID_SIZE + j] - (grid[(i+1) * GRID_SIZE + j] + grid[(i-1) * GRID_SIZE + j] + grid[i * GRID_SIZE + j+1] + grid[i * GRID_SIZE + j-1])/4) >= 0.1) {
				if (!((i == 400 && j <= 330) || (i == 200 && j == 500) || (i % 20 == 0 || j % 20 == 0))) {
					return 0;
				}
			}
		}
	}
	return 1;
}

void set_constant_temps(float* grid) {
	int i, j;
	for (i = 0; i < GRID_SIZE; i++) {
		grid[(GRID_SIZE - 1) * GRID_SIZE + i] = 100;
		grid[i] = 0;
		grid[i * GRID_SIZE + 0] = 0;
		grid[i * GRID_SIZE + GRID_SIZE - 1] = 0;
	}

	for (i = 1; i <= 330; i++) {
		grid[400 * GRID_SIZE + i] = 100;
	}

	grid[200 * GRID_SIZE + 500] = 100;

	// Every 20 rows = 100
  for(i = 0; i < GRID_SIZE; i++) {
    if((i % 20) == 0) {
      for(j = 0; j < GRID_SIZE; j++) {
        grid[i * GRID_SIZE + j] = 100;
      }
    }
  }

  // Every 20 cols = 0
  for(j = 0; j < GRID_SIZE; j++) {
    if((j % 20) == 0) {
      for(i = 0; i < GRID_SIZE; i++) {
        grid[i * GRID_SIZE + j] = 0;
      }
    }
  }
}

void * hotplate(void * ptr) {
	int i, j, iterations;
	int startIndex = *((int*)ptr);

	// initialize the grid
	for (i = startIndex; i < THREAD_LAST_ROW(startIndex); i++) {
		for (j = 0; j < GRID_SIZE; j++) {
			grid[i * GRID_SIZE + j] = 50;
			oldGrid[i * GRID_SIZE + j] = 50;
		}
	}

	// TODO - add the constant cells
	linear_barrier_wait(&barrier);
	set_constant_temps(grid);
	set_constant_temps(oldGrid);

	// TODO - add a barrier here for all of the threads to synch up before actually starting the iterations part
	iterations = 0;
	do {
		for (i = 1; i < GRID_SIZE - 1; i++) {
			for (j = 1; j < GRID_SIZE - 1; j++) {
				grid[i * GRID_SIZE + j] = (oldGrid[(i+1) * GRID_SIZE + j] + oldGrid[(i-1) * GRID_SIZE + j] + oldGrid[i * GRID_SIZE + j+1] + oldGrid[i * GRID_SIZE + j-1] + 4 * oldGrid[i * GRID_SIZE + j]) / 8;
			}
		}
		set_constant_temps(grid);

		temp = oldGrid;
		oldGrid = grid;
		grid = temp;
		iterations++;
		printf("Iteration %d\n\n", iterations);
	} while (!steady(oldGrid));

	printf("Total iterations: %d\n", iterations);
}

int main(int argc, char** argv) {
	int iterations, i;
	double startTime, endTime;

	// get start time
	startTime = when();

	// allocate GRID_SIZE x GRID_SIZE grid
	grid = (float*) malloc(GRID_SIZE * GRID_SIZE * sizeof(float));
	oldGrid = (float*) malloc(GRID_SIZE * GRID_SIZE * sizeof(float));

	// initialize the barrier that all of the threads will use to synchronize
	// pthread_barrier_init(&barrier, NULL, NUM_THREADS);
	linear_barrier_init(&barrier);

	// create the threads
	pthread_t threads[NUM_THREADS];

	for (i = 0; i < NUM_THREADS; i++) {
		int startIndex = i * ROWS_PER_THREAD;
		pthread_create(&threads[i], NULL, hotplate, &startIndex);
	}

	// actually run the simulation

	// wait for all threads to terminate
	for (i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	endTime = when();
	printf("Total time: %f\n\n", endTime - startTime);
}
