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

void sendFile(int serv){
    char* fs_name = "./client_send.txt";
    char sdbuf[LENGTH];
    printf("[Client] Sending %s to the Server... ", fs_name);
    FILE *fs = fopen(fs_name, "r");
    if(fs == NULL)
    {
        printf("ERROR: File %s not found.\n", fs_name);
        exit(1);
    }

    bzero(sdbuf, LENGTH);
    int fs_block_sz;
    while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs)) > 0)
    {
        if(send(serv, sdbuf, fs_block_sz, 0) < 0)
        {
            fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", fs_name, errno);
            break;
        }
        bzero(sdbuf, LENGTH);
    }
    printf("Ok File %s from Client was Sent!\n", fs_name);
}
void receiveFile(int serv){
    char revbuf[LENGTH];
    printf("[Client] Receiveing file from Server and saving it as final.txt...");
    char* fr_name = "./client_receive.txt";
    FILE *fr = fopen(fr_name, "a");
    if(fr == NULL)
        printf("File %s Cannot be opened.\n", fr_name);
    else
    {
        bzero(revbuf, LENGTH);
        int fr_block_sz = 0;
        while((fr_block_sz = recv(serv, revbuf, LENGTH, 0)) > 0)
        {
            int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
            bzero(revbuf, LENGTH);
            if (fr_block_sz == 0 || fr_block_sz != 512)
            {
                break;
            }
        }
        if(fr_block_sz < 0)
        {
            if (errno == EAGAIN)
            {
                printf("recv() timed out.\n");
            }
            else
            {
                fprintf(stderr, "recv() failed due to errno = %d\n", errno);
            }
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
            sendFile()
            /* create message */
            snprintf(message,26, "Greetings from process %d!", my_rank);
            /* send message to thread 0 */
            MPI_Send(message, strlen(message)+1, MPI_CHAR,dest, tag, MPI_COMM_WORLD);
        }
        else  //IF I'M THREAD 0
        {
            printf("Number of processes in system: %d\n",num_procs);
            for (source = 1; source < num_procs; source++)
            {
                MPI_Recv(message, 100, MPI_CHAR, source, tag,MPI_COMM_WORLD, &status);
                printf("Process 0 received %s from %d\n",message,source);
            }
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