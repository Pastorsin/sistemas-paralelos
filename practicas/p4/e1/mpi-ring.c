/*
** Sending simple, point-to-point messages.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

#define MASTER 0

int main(int argc, char* argv[])
{
  int myrank;
  int size;
  int dest;              /* destination rank for message */
  int source;            /* source rank of a message */
  int tag = 0;           /* scope for adding extra information to a message */
  MPI_Status status;     /* struct used by MPI_Recv */
  char my_message[BUFSIZ];
  char dest_message[BUFSIZ];

  /* MPI_Init returns once it has started up processes */
  MPI_Init( &argc, &argv );

  /* size and rank will become ubiquitous */
  MPI_Comm_size( MPI_COMM_WORLD, &size );
  MPI_Comm_rank( MPI_COMM_WORLD, &myrank );

  /*
  ** SPMD - conditionals based upon rank
  ** will also become ubiquitous
  */

  /* create a message to send, in this case a character array */
  sprintf(my_message, "Hola Mundo! Soy el proceso %d!", myrank);

  dest = (myrank + 1) % size;
  source = (myrank - 1) % size;

  if (myrank % 2) {  /* odd process do send-receive */
    MPI_Send(my_message, strlen(my_message) + 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
    MPI_Recv(dest_message, BUFSIZ, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
    printf("Mensaje recibido (process %d): %s\n", myrank, dest_message);

  }
  else {  /* peer process do receive-send */
    MPI_Recv(dest_message, BUFSIZ, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
    MPI_Send(my_message, strlen(my_message) + 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
    printf("Mensaje recibido (process %d): %s\n", myrank, dest_message);
  }

  /* don't forget to tidy up when we're done */
  MPI_Finalize();

  /* and exit the program */
  return EXIT_SUCCESS;
}