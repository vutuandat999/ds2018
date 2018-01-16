#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define LENGTH 512


int receive(int cli){
    char revbuf[LENGTH];

    char* fr_name = "./server_receive.txt";
    FILE *fr = fopen(fr_name, "a");
    if(fr == NULL)
        printf("File %s Cannot be opened file on server.\n", fr_name);
    else
    {
        bzero(revbuf, LENGTH);
        int fr_block_sz = 0;
        //int success = 0;
        //while(success == 0)
        //{
        while(fr_block_sz = recv(cli, revbuf, LENGTH, 0)) //could it be sockfd?
        {
            int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
            if(fr_block_sz)
            {
                break;
            }
            bzero(revbuf, LENGTH);
        }
        printf("Ok received from client!\n");
        fclose(fr);
        //}
    }
}

int sendMessage(int cli) {
    char* fs_name = "./server_send.txt";
    char sdbuf[LENGTH]; // Send buffer
    printf("[Server] Sending %s to the Client...", fs_name);
    FILE *fs = fopen(fs_name, "r");
    if(fs == NULL)
    {
        printf("ERROR: File %s not found on server.\n", fs_name);
        exit(1);
    }

    bzero(sdbuf, LENGTH);
    int fs_block_sz;
    while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs))>0)
    {
        if(send(cli, sdbuf, fs_block_sz, 0) < 0)
        {
            printf("ERROR: Failed to send file %s.\n", fs_name);
            exit(1);
        }
        bzero(sdbuf, LENGTH);
    }
    printf("Ok sent to client!\n");
}

int main() {
    int ss, cli, pid;
    struct sockaddr_in ad;
    char s[100];
    socklen_t ad_length = sizeof(ad);

    // create the socket
    // AF_INET for address family ipv4
    // SOCK_STREAM for TCP
    ss = socket(AF_INET, SOCK_STREAM, 0);

    // bind the socket to port 12345
    memset(&ad, 0, sizeof(ad));
    ad.sin_family = AF_INET;
    ad.sin_addr.s_addr = INADDR_ANY;
    ad.sin_port = htons(12345);
    bind(ss, (struct sockaddr *)&ad, ad_length);

    // then listen
    listen(ss, 0);

    while (1) {
        // an incoming connection
        cli = accept(ss, (struct sockaddr *)&ad, &ad_length);

        pid = fork();
        if (pid == 0) {
            // I'm the son, I'll serve this client
            printf("client connected\n");
            while (1) {
                // it's client turn to chat, I wait and read message from client
//                read(cli, s, sizeof(s));
//                printf("client says: %s\n",s);
                receive(cli);

                // now it's my (server) turn
//                printf("server>", s);
//                scanf("%s", s);
//                write(cli, s, strlen(s) + 1);
                sendMessage(cli);
            }
            return 0;
        }
        else {
            // I'm the father, continue the loop to accept more clients
            continue;
        }
    }
    // disconnect
    close(cli);

}