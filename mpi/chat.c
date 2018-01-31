#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <mpi.h>
#define LENGTH 512

void sendFile(int dest, int tag){
    char* fs_name = "./client_send.txt";
    char sdbuf[LENGTH];
    printf("[Client] Sending %s to the Server... ", fs_name);
    FILE *fs = fopen(fs_name, "r");
    if(fs == NULL)
    {
        printf("ERROR: File %s not found.\n", fs_name);
        exit(1);
    }

    fread(sdbuf, sizeof(char), LENGTH, fs);
    /* send message to thread 0 */
    MPI_Send(sdbuf, strlen(sdbuf)+1, MPI_CHAR,dest, tag, MPI_COMM_WORLD);
    printf("Ok File %s from Client was Sent!\n", fs_name);
}
void receiveFile(int source, int num_procs, int tag, MPI_Status status){
    char revbuf[LENGTH];
    printf("[Client] Receiveing file from Server and saving it as final.txt...");
    char* fr_name = "./client_receive.txt";
    FILE *fr = fopen(fr_name, "a");
    if(fr == NULL)
        printf("File %s Cannot be opened.\n", fr_name);
    else
    {
        for (source = 1; source < num_procs; source++)
        {
            MPI_Recv(revbuf, 100, MPI_CHAR, source, tag,MPI_COMM_WORLD, &status);
            fwrite(revbuf, sizeof(char), sizeof(revbuf), fr);
            printf("Process 0 received %s from %d\n",revbuf,source);
        }
        printf("Ok received from server!\n");
        fclose(fr);
    }
}


int main(int argc, char* argv[]) {
    char s[100];

    int			my_rank;		/* rank of process */
	int			num_procs;		/* number of processes */
	int			source;			/* rank of sender */
	int			dest = 0;		/* rank of receiver */
	int			tag = 0;		/* tag for messages */
	char		message[100];	/* storage for message */
    MPI_Status  status ; /* return status for receive */

    /* start up MPI */
	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs); 

    printf("Process %d\n",my_rank);

    while (1) {
        if (my_rank != 0)   //IF I'M ANOTHER THREAD
        {
            sendFile(dest, tag);
        }
        else  //IF I'M THREAD 0
        {
            printf("Number of processes in system: %d\n",num_procs);
            receiveFile(source, num_procs, tag, status);
        }

        printf("Continue?(Y/N)\n");
        printf("client>");
        scanf("%s", s);
        if (s[0] == 'N'){
            printf("[Client] Connection lost.\n");
            return 0;
        }

    }
}