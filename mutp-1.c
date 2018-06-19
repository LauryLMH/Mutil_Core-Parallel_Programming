#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#define N 2000000

int comm_sz;
int rank;
clock_t begin_init, begin_calc, end;
double time_serial_from_init, time_serial_from_calc, time_parallel_from_init, time_parallel_from_calc;

// Method declarations
void serialPCC();
void parallelPCC();

int main(void)
{
	MPI_Init(&argc, &argv); //For C language calls need to give parameters argc and argv
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); //MPI call interface
	MPI_Comm_size(MPI_COMM_WORLD, &size);//A number of processes within the communication domain comm


	// Executing the parallel method.
	// If statement to make sure it is only executed when > 1 process exists.
	if (comm_sz > 1) {
		parallelPCC();
	}
	else {
		printf("\nERROR. Cannot carry out parallel implementation since you chose to run less than 2 processes.\n\n");
	}

	MPI_Finalize();
	return 0;
}



 void parallelPCC() {

	 // Declaration of global input arrays and other variables.
	 double* a = malloc((N) * sizeof(double));
	 double* b = malloc((N) * sizeof(double));
	 double tempSumA = 0;
	 double tempSumB = 0;
	 double totalA = 0;
	 double totalB = 0;
	 double meanA = 0;
	 double meanB = 0;
	 double lastB = 0;


	 // Each local array is then initialized and the total of local a
	 // summed, just as in the serial method above.
	 for (int i = 0; i < N; i++) {

		 if (i == 0) {
			 a[i] = sin(i);
		 }

		 b[i] = sin(i + 1);

		 if (i < N - 1) {
			 a[i + 1] = b[i];
		 }

		 totalA += a[i];
	 }

	 totalB = totalA + b[N - 1] - a[0];
	 // b[MAXLENGTH-1] must be sent back to process 0 so that it can calculate
	 // the total of array b (since 'totalB = totalA + b[MAXLENGTH-1]')
	 if (rank == comm_sz - 1) {
		 lastB = b[N - 1];
		 MPI_Send(&lastB, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	 }

	 // Reducing all the local temproary summations into 1 overall value for totalA.
	 MPI_Reduce(&tempSumA, &totalA, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	 // Store the starting calculation clock time
	 begin_calc = clock();

	 // Process 0 now receives b[MAXLENGTH-1] and calculates the means
	 if (rank == 0) {

		 MPI_Recv(&lastB, 1, MPI_DOUBLE, comm_sz - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		 meanA = totalA / N;

		 totalB = totalA + lastB;
		 meanB = totalB / N;

		 printf("\nMean A: %.12f", meanA);
		 printf("\nMean B: %.12f\n", meanB);
	 }

	 // Broadcasting the calculated means back to all other processes
	 MPI_Bcast(&meanA, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	 MPI_Bcast(&meanB, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);


	 // Declaring variables needed to calculate standard deviation and pearson correlation coefficient.
	 double tempA = 0;
	 double tempB = 0;
	 double tempSum = 0;
	 double totalA2 = 0;
	 double totalB2 = 0;
	 double totalTempSum = 0;

	 /*
	 * This loop calculates the numerator in both the standard deviation and pearson
	 * correlation coefficient calcualtions.
	 * Works in exactly same way as loop in serial method, except each process only
	 * does calculations for it local array.
	 */
	 for (int j = 0; j < N ; j++) {

		 tempA += (a[j] - meanA) * (a[j] - meanA);
		 tempB += (b[j] - meanB) * (b[j] - meanB);

		 // Pearson
		 tempSum += (a[j] - meanA) * (b[j] - meanB);
	 }

	 // Locally calculated values from each process are then reduced to an overall sum
	 MPI_Reduce(&tempA, &totalA2, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	 MPI_Reduce(&tempB, &totalB2, 1, MPI_DOUBLE, MPI_SUM, 1, MPI_COMM_WORLD);
	 MPI_Reduce(&tempSum, &totalTempSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	 // Releasing the memory of the arrays
	 free(a);
	 free(b);

	 // Process 1 calculates the standard deviation of B and then sends this
	 // value to process 0.
	 // Process 0 calculates the standard deviation of A.
	 double sdB = 0;
	 if (rank == 1) {
		 sdB = sqrt(totalB2 / N);
		 MPI_Send(&sdB, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	 }
	 if (rank == 0) {
		 double sdA = sqrt(totalA2 / N);
		 printf("\nStandard Deviation A: %.12f", sdA);

		 MPI_Recv(&sdB, 1, MPI_DOUBLE, 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		 printf("\nStandard Deviation B: %.12f\n", sdB);

		 // Calculating the pearson correlation coefficient
		 totalTempSum = totalTempSum / N;
		 double pcc = totalTempSum / (sdA * sdB);
		 printf("\nPearson Correlation Coefficient: %.12f\n", pcc);

		 // Take ending clock time and print time taken
		 end = clock();
		 time_parallel_from_calc = ((double)(end - begin_calc) / CLOCKS_PER_SEC) * 1000;
		 printf("\nTime Taken (Calculation Only): %.3f ms", time_parallel_from_calc);
		 time_parallel_from_init = ((double)(end - begin_init) / CLOCKS_PER_SEC) * 1000;
		 printf("\nTime Taken (Including Array Initialization): %.3f ms\n", time_parallel_from_init);

		 printf("\n====================== END PARALLEL ======================\n");
	 }

 }
 