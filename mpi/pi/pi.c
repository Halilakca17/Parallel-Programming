#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char* argv[]) {
    int n, myid, numprocs, i, j, k, l;
    double PI25DT = 3.14159265358979323846264338327950288419716;
    double mypi, pi, h, sum, x;
    double dummy = 0.0;
    double t1, t2;

    MPI_Init(&argc, &argv);                         
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);       // Get number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);           // Get this process's rank (ID)

    t1 = MPI_Wtime();                               

    n = 100;                                       
    h = 1.0 / (double) n;                            
    sum = 0.0;

    // Each process computes a portion of the sum
    for (i = myid + 1; i <= n; i += numprocs) {
        x = h * ((double)i - 0.5);                   
        sum += (4.0 / (1.0 + x * x));               

        // Artificial CPU load to simulate computation time
        for(j = 0; j < 1000; j++)
            for(k = 0; k < 1000; k++)
                for(l = 0; l < 100; l++)
                    dummy = dummy + j + k + l;
    }

    mypi = h * sum;                                  // Local approximation of pi

    // Collect results from all processes and sum them in process 0
    MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Each process prints its own intermediate result
    printf("Myid is %d: mysum: %5.2f dummy: %5.3f\n", myid, sum, dummy);

    // Process 0 prints final result and error
    if (myid == 0)
        printf("pi is %.25f, Error is %.25f\n\n\n\n", pi, fabs(pi - PI25DT));

    t2 = MPI_Wtime();                               

    // Each process reports its elapsed time
    printf("Elapsed time [%2d] is %8.2f\n", myid, t2 - t1);

    MPI_Finalize();                                 
    return 0;
}
