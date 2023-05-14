#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, NumOfProc;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &NumOfProc);

    int i, j;

    double begin_time = MPI_Wtime();

    double tau = 1e-3;  // шаг по оси времени
    double h = 1e-3;  // шаг по оси x
    double T = 10., X = 10.;  // рассмотрим квадрат (x; t) ∈ [0; 10]х[0; 10] 
    double sigma = tau / h;  

    int NumOfSegments_t = (int) (T / tau);
    int NumOfSegments_x = (int) (X / h);

    int NumOfNodes_t = NumOfSegments_t + 1;
    int NumOfNodes_x = NumOfSegments_x + 1;

    // инициализируем граничные и начальные условия
    double* bound_cond = (double*) malloc (NumOfNodes_t * sizeof(double));
    double* start_cond = (double*) malloc (NumOfNodes_x * sizeof(double));

    for (i = 0; i < NumOfNodes_t; i++)
        bound_cond[i] = cos(1000. * i * tau);

    for (i = 0; i < NumOfNodes_x; i++)
        start_cond[i] = cos(1000. * i * h);
    // все процессы, кроме последнего, имеют одинаковое количество узлов
    int NumOfSegments_x_for_1proc = NumOfSegments_x / NumOfProc;
    int start_node = rank * NumOfSegments_x_for_1proc;
    int end_node;
    // если процесс последний, он берет все оставшиеся узлы
    if (rank == NumOfProc - 1)
        end_node = NumOfNodes_x;
    else
        end_node = (rank + 1) * NumOfSegments_x_for_1proc - 1;

    printf("ранк %d, стартовый узел: %d, конечный: %d\n", rank, start_node, end_node);

    int length_of_array = end_node - start_node + 1;
    // 'u' is an array 2x(end_node - start_node + 1) - it saves the last and preultimate layers
    double** u = (double**) malloc (2 * sizeof(double*));
    for (i = 0; i < 2; i++)
        u[i] = (double*) malloc (length_of_array * sizeof(double));
    // используем начальные условия
    for (i = 0; i < length_of_array; i++)
        u[0][i] = start_cond[rank * NumOfSegments_x_for_1proc + i];

    for (j = 0; j < NumOfSegments_t; j++) {
        // явная 4-точечная схема
        for (i = 1; i < length_of_array - 1; i++)
            u[1][i] = u[0][i] + 0.5 * pow(sigma, 2) * (u[0][i-1] - 2*u[0][i] + u[0][i+1]) - 0.5 * sigma * (u[0][i+1] - u[0][i-1]);
        // граничное условие
        if (rank == 0)
            u[1][0] = bound_cond[j+1];
        // если процесс последний, он вычисляет правые узлы через левый угол
        if (rank == NumOfProc - 1)
            u[1][length_of_array-1] = u[0][length_of_array-1] - sigma * (u[0][length_of_array-1] - u[0][length_of_array-2]);
        
// каждый процесс, кроме последнего, отправляет нужное число своему соседу справа
        if (rank != NumOfProc - 1)
            MPI_Send(&u[0][length_of_array-1], 1, MPI_DOUBLE, rank + 1, 125, MPI_COMM_WORLD);
        
// каждый процесс, кроме нулевого, получает число от своего левого соседа
        if (rank != 0) {
            double tmp_left;
            MPI_Recv(&tmp_left, 1, MPI_DOUBLE, rank - 1, 125, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // полученное число сразу используется в расчете
            u[1][0] = u[0][0] + 0.5 * pow(sigma, 2) * (u[0][1] - 2*u[0][0] + tmp_left) - 0.5 * sigma * (u[0][1] - tmp_left);
            // each process, except the zeroth one, sends a necessary number to its left neighbour
            MPI_Send(&u[0][0], 1, MPI_DOUBLE, rank - 1, 225, MPI_COMM_WORLD);
        }
        // each process, except the last one, receives a number from its right neighbour
        if (rank != NumOfProc - 1) {
            double tmp_right;
            MPI_Recv(&tmp_right, 1, MPI_DOUBLE, rank + 1, 225, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            u[1][length_of_array-1] = u[0][length_of_array-1] + 0.5 * pow(sigma, 2) * (tmp_right - 2*u[0][length_of_array-1] + u[0][length_of_array-2]) - 0.5 * sigma * (tmp_right - u[0][length_of_array-2]);
        }
        // prepare for the next iteration (make shift)
        for (i = 0; i < length_of_array; i++)
            u[0][i] = u[1][i];
    }
    
    double sum_of_deviations = 0.;
    double* analytical_answer = (double*) malloc (length_of_array * sizeof(double));
    for (i = 0; i < length_of_array; i++) {
        analytical_answer[i] = cos(1000. * (h * (rank * NumOfSegments_x_for_1proc + i) - T));
        sum_of_deviations += abs(analytical_answer[i] - u[1][i]);
    }
    double middle_deviation = sum_of_deviations / length_of_array;
    printf("rank: %d; middle deviation: %.15lf\n", rank, middle_deviation);

    double end_time = MPI_Wtime();
    if (rank == 0)
        printf("Elapsed time: %lf\n", end_time - begin_time);

    // free allocated memory
    free(bound_cond);
    free(start_cond);
    for (i = 0; i < 2; i++)
        free(u[i]);
    free(u);
    free(analytical_answer);
    MPI_Finalize();
    return 0;
}
