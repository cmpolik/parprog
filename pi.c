#include <stdio.h>
#include <math.h>
#include "mpi.h"
#define TAG 5

double pi_sum(int n, int m){
	double pi, s = 0;
	for(int i = n; i < m+1; i++){
		(i%2 == 0)?(s = 1):(s = -1);
		pi += s/(2*i + 1);
	}
	return pi*4;
}

int main(int argc, char **argv){
	
	int rank = 0; 	
	int size = 0;											
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);   		
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	
	const int N = 10e5;
	double part = 0; 
	

	part = pi_sum(rank*N/size, (rank+1)*N/size);
	//printf("%d %d %d %lf\n", rank, rank*N/size, (rank+1)*N/size, part);
	//part = pi_sum(0, N);	
	if (rank != 0) {
		//printf("on %d rank the part is %lf \n", rank, part);
		MPI_Send(&part, 1, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);		
	}
	
	if (rank == 0) {
		double pi = 0;
		for (int i = 1; i < size; i++){
			double tmp;
			MPI_Recv(&tmp, 1, MPI_DOUBLE, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);
			pi += tmp;
		}
		pi  += part;
		printf("rank %d %f \n", rank, pi);
	}
	
	MPI_Finalize();
	//printf("первая сумма - %f, вторая сумма - %f \n", pi_sum(0*N, N), pi_sum(N, 2*N));
}

