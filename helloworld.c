#include<stdio.h>
#include <string.h>
#include"mpi.h"
#define TAG 1

int main(int argc, char **argv) {
	const char* text1 = "Hello World!";
	int rank = 0; 					// номер ноды
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	if( rank == 0){
	
		MPI_Send(text1, strlen(text1), MPI_CHAR, 1, TAG, MPI_COMM_WORLD);
	}

	if( rank == 1){
		char buffer[12];
		MPI_Recv(buffer, 12, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &status);
		
		printf("%s\n", buffer);
	}
	MPI_Finalize();
}
