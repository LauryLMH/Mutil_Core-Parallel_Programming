// ParallelPI.cpp : Defines the entry point for the MPI application.
//
#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include "limits.h"
#include <ppl.h>
#include <random>
#include <time.h>

using namespace Concurrency;

int ThrowDarts(int iterations)
{

combinable<int> count;

int result = 0;


parallel_for(0, iterations, [&](int i){

std::tr1::uniform_real<double> MyRandom;
double RandMax = MyRandom.max();
std::tr1::minstd_rand0 MyEngine;
double x, y;

MyEngine.seed((unsigned int)time(NULL));

x = MyRandom(MyEngine)/RandMax;
y = MyRandom(MyEngine)/RandMax;
  
if(x*x + y*y < 1.0)
{
count.local() += 1;
}
});

result = count.combine([](int left, int right) { return left + right; });

return result;
}

void main(int argc, char* argv[])
{
int rank;
int size;
int iterations;
int count;
int result;

MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD,&rank);
MPI_Comm_size(MPI_COMM_WORLD,&size);

if(rank == 0)
{
//Rank 0 reads the number of iterations from the command line.
//50M iterations is the default.
iterations = 50000000;
if(argc > 1)
{
iterations = atoi(argv[argc-1]);
}
printf("Executing %d iterations on %d nodes.\n", iterations, size);
fflush(stdout);
}
//Broadcast the number of iterations to execute.
MPI_Bcast(&iterations, 1, MPI_INT, 0, MPI_COMM_WORLD);

count = ThrowDarts(iterations);

//Gather and sum results
MPI_Reduce(&count, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

if(rank == 0)
{
printf("The value of PI is approximated to be: %16f", 4*((double)result/(double)(iterations*size)));
}
    
MPI_Barrier(MPI_COMM_WORLD);

MPI_Finalize();

}