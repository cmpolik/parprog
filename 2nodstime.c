#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#define TAG 123

int main(int argc, char **argv) {
	
	int rank, size;
  	MPI_Status status;
  	MPI_Init(&argc, &argv);                  							// инициализация MPI
 	MPI_Comm_rank(MPI_COMM_WORLD, &rank);      							// получение идентификатора узла
 	MPI_Comm_size(MPI_COMM_WORLD, &size);       						// получение количества узлов

	char *text1 = calloc(120, sizeof(char));
	for (int i = 0; i < sizeof(text1); i++)
		text1[i] = i;
	
	
	char buffer[sizeof(text1)];		// для Hello!
	
	if( rank == 0){
		// запускаем время на всех не 0ых узлах
		//time_start();
		double start, stop;
		for (int i = 0; i<120; i++) {
			start = MPI_Wtime();
			MPI_Send(&text1[i], sizeof(text1), MPI_CHAR, 1, TAG, MPI_COMM_WORLD);
			MPI_Recv(buffer, sizeof(text1), MPI_CHAR, 1, TAG, MPI_COMM_WORLD, &status);
			stop = MPI_Wtime();
			printf("%d %lf   \n", i, stop - start);
		}
		
	}

	if( rank == 1){
		for (int i = 0; i<120; i++){	
			MPI_Recv(buffer, sizeof(text1), MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &status);
			MPI_Send(text1, sizeof(text1), MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
		}
		//while (rank != 0){
		//	printf("%s from %d here's my message time %ld \n", buffer, rank, time_stop());
		//}
		//printf("%s\n", buffer);
	}

	MPI_Finalize();
  	return 0;
}




