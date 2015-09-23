#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define GRID_SIZE 4096

int convergedX = 0, convergedY = 0;

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
	#pragma omp parallel for private(i, j)
	for (i = 1; i < GRID_SIZE - 1; i++) {
		for (j= 1; j < GRID_SIZE - 1; j++) {
			if (abs_val(grid[i * GRID_SIZE + j] - (grid[(i+1) * GRID_SIZE + j] + grid[(i-1) * GRID_SIZE + j] + grid[i * GRID_SIZE + j+1] + grid[i * GRID_SIZE + j-1])/4) >= 0.1) {
				if (!((i == 400 && j <= 330) || (i == 200 && j == 500))) {
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
}

int main(int argc, char** argv) {
	int iterations;
	double startTime, endTime;
	float *temp, *grid, *oldGrid;

	// get start time
	startTime = when();

	// allocate GRID_SIZE x GRID_SIZE grid
	grid = (float*) malloc(GRID_SIZE * GRID_SIZE * sizeof(float));
	oldGrid = (float*) malloc(GRID_SIZE * GRID_SIZE * sizeof(float));

	// initialize the grid
	int i, j;
	#pragma omp parallel for private(i, j)
	for (i = 0; i < GRID_SIZE; i++) {
		for (j = 0; j < GRID_SIZE; j++) {
			grid[i * GRID_SIZE + j] = 50;
			oldGrid[i * GRID_SIZE + j] = 50;
		}
	}
	set_constant_temps(grid);
	set_constant_temps(oldGrid);

	// actually run the simulation
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
	} while (!steady(oldGrid));
	endTime = when();
	printf("Total iterations: %d\n", iterations);
	printf("Total time: %f\n\n", endTime - startTime);
}
