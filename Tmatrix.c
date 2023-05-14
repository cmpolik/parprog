#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#define TAG 6

int main(int argc, char **argv) {
	
	srand(time(NULL));
	
	int rank, size;
  	MPI_Status status;
  	MPI_Init(&argc, &argv);                  							// инициализация MPI
 	MPI_Comm_rank(MPI_COMM_WORLD, &rank);      							// получение идентификатора узла
 	MPI_Comm_size(MPI_COMM_WORLD, &size);       						// получение количества узлов

  	
  	int rows = TAG;		                            					// матрица размера 
  	int cols = TAG;                                						
  	
  	// выделить память под матрицы
	int *matrix, *t_matrix;
  	matrix = calloc(rows * cols, sizeof(int));
 	t_matrix = calloc(rows * cols, sizeof(int));


  	// заполнить матрицу случайными числами от 0 до 9
  	for (int i = 0; i < rows * cols; i++) 
   		matrix[i] = rand() % 10;


  	// вывести изначальную матрицу
  	for (int i = 0; i < rows; i++) {
    	for (int j = 0; j < cols; j++) {
      		printf("%d ", matrix[i * cols + j]);
   		}
   		printf("\n");
  	}
  	printf("\n");

  	// транспонировать матрицу на каждом узле
  	for (int i = 0; i < rows; i++) {
    	for (int j = 0; j < cols; j++) {
      		t_matrix[j * rows + i] = matrix[i * cols + j];
    	}
  	}

  	// передать транспонированную матрицу на другой узел с помощью MPI_Send и MPI_Recv
  	if (rank == 0) {
    	MPI_Send(t_matrix, rows * cols, MPI_INT, 1, TAG, MPI_COMM_WORLD);
 	} else if (rank == 1) {
   		MPI_Recv(t_matrix, rows * cols, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
  	}

  	// вывести транспонированную матрицу на каждом узле после передачи
  	printf("матрица на узле %d после транспанирования:\n", rank);
  	for (int i = 0; i < rows; i++) {
    	for (int j = 0; j < cols; j++) {
      		printf("%d ", t_matrix[i * cols + j]);
    	}
    	printf("\n");
  	}
  	printf("\n");

 	MPI_Finalize();
  	return 0;
}
