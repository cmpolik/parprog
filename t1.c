#include <stdio.h>
#include <string.h>

#include "mpi.h"


int main(int argc, char ** argv){

    int rank, size;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    double part = 1.0 / size; 

    if( rank != 0 ){
        printf( "rank: %d, part: %lf\n", rank, part);
        MPI_Send( &part, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD );
    }

    if( rank == 0 ){
        double sum = part;

        for( int i = 0 ; i < size - 1 ; ++i ){
            double buffer;
            MPI_Recv( &buffer, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
            sum += buffer;
        }

        printf( "result: %lf\n", sum );
    }

    MPI_Finalize();


}