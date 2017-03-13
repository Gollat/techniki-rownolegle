#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_MESSAGE_SIZE (1<<21)
#define BYTES_IN_MEGABIT (1<<17)

#define MEASUREMENTS_NUMBER 10

int main(int argc, char **argv)
{
    int rank, numprocs, message_size, buffer_size, i;

    char *message, *buffer;

    double start, end, total_time, average_time;
    double message_size_mbits, bandwidth;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    if (numprocs != 2)
    {
        if(rank == 0)
        {
            fprintf(stderr, "This test requires exactly two processes!\n");
        }

        MPI_Finalize();
        return EXIT_FAILURE;
    }

    if (rank == 0)
    {
        printf("MESSAGE SIZE IN BITS\tRUNTIME IN s\tBANDWIDTH IN MBITS/s\n");
    }

    for (message_size = 1; message_size < MAX_MESSAGE_SIZE; message_size *= 2)
    {
        MPI_Pack_size(message_size, MPI_BYTE, MPI_COMM_WORLD, &buffer_size);
        buffer_size += MPI_BSEND_OVERHEAD;

        message = (char *) malloc(message_size * sizeof(char));
        buffer  = (char *) malloc(buffer_size);

        MPI_Buffer_attach(buffer, buffer_size);

        total_time = 0;

        for (i = 0; i < MEASUREMENTS_NUMBER; i++)
        {
            MPI_Barrier(MPI_COMM_WORLD);

            if (rank == 0)
            {
                start = MPI_Wtime();

                MPI_Bsend(message, message_size, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
                MPI_Recv(message, message_size, MPI_BYTE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                end = MPI_Wtime();

                total_time += (end - start) / 2;
            }
            else
            {
                MPI_Recv(message, message_size, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Bsend(message, message_size, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
            }
        }

        if (rank == 0)
        {
            average_time = total_time / MEASUREMENTS_NUMBER;
            message_size_mbits = message_size * 1.0 / BYTES_IN_MEGABIT;
            bandwidth = message_size_mbits / average_time;
            printf("%i\t%f\t%f\n", message_size, average_time, bandwidth);
        }
        MPI_Buffer_detach(buffer, &buffer_size);
        free(buffer);
        free(message);
    }
    MPI_Finalize();
    return 0;
}
