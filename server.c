#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define MAX 1024
#define SERVER_PORT 12345

   

void readData(int soc){
    int e;
    while(1){
        char buff[MAX];
        e = read(soc,buff, MAX);
        if(e < 0){
            perror("reading");
        }
        if(strncmp(buff,"~~~quit~~~",10) == 0){
            return;
        }
        printf("%s", buff);
        bzero(buff, MAX);
    }
}
   
int main()
{
    int serverSoc, clientSoc ;
    struct sockaddr_in servaddr;
   
    serverSoc = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSoc < 0) {
        perror("");
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
   
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT);
   
    if ((bind(serverSoc, (struct sockaddr*) &servaddr, sizeof(servaddr))) != 0) {
        perror("binding falied");
        exit(1);
    }

    if ((listen(serverSoc, 2)) != 0) {
        exit(1);
    }

    
    while(1){//main loop to accept clients
        clientSoc = accept(serverSoc, NULL, NULL);
        if (clientSoc < 0) {
            exit(1);
        }
        readData(clientSoc);// get data from single client
    }
    close(serverSoc);
}