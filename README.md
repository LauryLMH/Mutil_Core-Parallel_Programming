# MPI cluster-based serial and parallel computing
Introduction：
This assignment mainly set three tasks: one is general calculation of serial programming, another one is to call the MPI with 2000000 arrays to output the Pearson correlation coefficient for parallel computing. Last one is make a comparison of their running time.

Analysis:
This program used two functions, one is parallel, another one is serial. In this case, they achieve similar functions.In the whole process, when only one process is processed, the serial function is used, when the amount of data to be processed (size) is greater than 1, the parallel function is used. In the parallel functions, the system automatically call the MPI and arrange the process for the program. In general terms, it is automatically assigned to four threads. 

At the beginning of the program, call the MPI interface. After that, if it is rank 0 process, then execute the serial function; if the exchange of communication is greater than the sum minus 1 which means process is greater than one, then implement the parallel functions. The process identification number returns the calling of communication process in the given domain,  different processes can be distinguished from themselves and other processes, achieving the parallel and collaborative process.

The loop of parallel calculates the numerator in Pearson, works in exactly same way as loop in serial method, except each process only does calculations for it local array.
On the whole structure, both of them put the global variables and other variables out of the main function to make it more easily modification.
In order to avoid process deadlock, the following methods are used in the program:
// Reducing all the local temproary summations into 1 overall value for totalA.
MPI_Reduce(&tempSumA, &totalA, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
MPI_Reduce(&tempSumB, &totalB, 1, MPI_DOUBLE, MPI_SUM, 1, MPI_COMM_WORLD);
// Broadcasting the calculated means back to all other processes
MPI_Bcast(&meanA, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
MPI_Bcast(&meanB, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

As an optimization, after the completion of the calculation of array, release the occupied memory.Re allocation of two processes for calculate the standard deviation. Finally, output the Pearson correlation coefficient. At last, end of timing, output operation time, compare the difference between serial and parallel. Therefore, parallel programming’s calculation is divided into four threads, so the speed increased by 75%.

＃基于MPI集群的串行和并行计算
介绍：
这个任务主要设置了三个任务：一个是串行编程的通用计算，另一个是用2000000个数组调用MPI输出并行计算的Pearson相关系数。最后一个是比较他们的运行时间。

分析：
这个程序使用了两个函数，一个是并行的，另一个是串行的。在这种情况下，它们实现类似的功能。在整个过程中，当只有一个进程被处理时，使用串行功能，当要处理的数据量（大小）大于1时，使用并行功能。在并行功能中，系统自动调用MPI并安排程序的进程。一般而言，它会自动分配给四个线程。

在程序开始时，调用MPI接口。之后，如果它是0级进程，则执行串行功能;如果通信交换大于总和减1，这意味着进程大于1，则执行并行功能。进程标识号返回给定域中的通信进程的调用，不同进程可以与自己和其他进程区分开来，实现并行协同进程。

并行循环计算Pearson中的分子，工作方式与串行方法中的循环完全相同，除了每个进程仅对其进行本地数组计算。
在整体结构上，它们都将全局变量和其他变量放在主函数之外，以使其更易于修改。
为了避免进程死锁，程序中使用了以下方法：
//将总体A的所有本地临时总和降为1总值。
MPI_Reduce（＆tempSumA，＆totalA，1，MPI_DOUBLE，MPI_SUM，0，MPI_COMM_WORLD）;
MPI_Reduce（＆tempSumB，＆totalB，1，MPI_DOUBLE，MPI_SUM，1，MPI_COMM_WORLD）;
//将计算出的方法广播回所有其他进程
MPI_Bcast（＆meanA，1，MPI_DOUBLE，0，MPI_COMM_WORLD）;
MPI_Bcast（＆meanB，1，MPI_DOUBLE，0，MPI_COMM_WORLD）;

作为优化，完成数组的计算后，释放占用的内存。重新分配两个进程来计算标准偏差。最后，输出皮尔逊相关系数。最后，计时结束，输出操作时间，比较串行和并行的差异。因此，并行编程的计算分为四个线程，所以速度提高了75％。
