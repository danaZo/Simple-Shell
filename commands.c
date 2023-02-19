#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <dirent.h>
#include <sys/wait.h>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>



#include "commands.h"

int sock = -1;
int defout = STDOUT;
FILE* stdout2 = NULL;
int sout_backup = 1; 
char cwd[PATH_MAX];

void shLoop(){
    //creating another handle to stdout so we can still write to it
    //when dircting the output to the server
    sout_backup = dup(1);
    stdout2 = fdopen(sout_backup,"w");

    char * command;
    getcwd(cwd, PATH_MAX);
    
    while(1){
        
        // fprintf(stdout2,"Insert a command:\n"); TASK a) commented as requested
        
        fprintf(stdout2,"%s:>", cwd);
        fflush(stdout2);

        //recieve command from the user
        command = getCommand();
        if(!command){
            perror("Allocating error");
            return;
        }
        //parse the command and execute it
        executeCommand(command);
        
    }

    free(command);
}
//recieve a command with unknown length from the user
char * getCommand(){

    size_t size = 10;
    int len = 0;
    char * s = (char*) malloc(size);
    if(!s){
        return s;
    }
    char c;

    while((c = getchar()) != '\n'){
        
        if(len == size - 1){
            // enlarge the string if maximum size reached
            s = realloc(s, (size += 10));
            if(!s){
                return s;
            }
        }
        s[len++] = c;

    }

    s[len++] = '\0';
    return realloc(s,len); // resize the string to it's accurate size
}

void executeCommand(char * cmd){
    
    

    if(strcmp("EXIT", cmd) == 0){ //part of task a)
        free(cmd);
        exit(0);
    }

    else if(strncmp("ECHO ", cmd, 5) == 0){  // task c) 
        cmd += 5;
        printf("%s\n", cmd);
        fflush(stdout);
    }
    else if(strcmp("TCP PORT", cmd) == 0){   //task d)
        //redirect the output to tcp socket
        if(defout == TCPOUT) return;

        connectToServer();
        defout = TCPOUT;
        dup2(sock, STDOUT_FILENO);
    }

    else if(strcmp("LOCAL", cmd) == 0){ // task e)
        
        if (defout == STDOUT) return;
        defout = STDOUT;

        //redirect the output stream to stdout
        printf("~~~quit~~~");
        fflush(stdout);
        close(sock);
        dup2(sout_backup, STDOUT_FILENO);
        
        //
    }
    else if(strcmp("DIR", cmd) == 0){  // task f) 
        
        DIR * curr = opendir("."); // open the current directory
        if(curr == NULL)
        {
            perror("Unable to read directory\n");
            return;
        }
        struct dirent * d;
        while((d = readdir(curr))){

            fprintf(stdout2,"%s\n", d->d_name);

        }
        closedir(curr);
        fflush(stdout2);
    } 
    else if(strncmp("CD ", cmd, 3) == 0){  // task g)
    // The chdir command is a system call.  
        cmd += 3;
        if(chdir(cmd) < 0){
           
            fprintf(stdout2,"Directory not found\n");
        }
        getcwd(cwd, PATH_MAX); // update the cwd to the new location
    }
    // else{ // task h) commented as requested
    //     system(cmd);

    // }
    //system is a library function
    else if(strncmp("COPY ", cmd, 5) == 0){  // task j)  
        // in this task we only used library methods,however some of them
        // has nested system calls

        FILE * sp, *dp;
        char *src, *dest, buff[2048];
        cmd += 5;
        
        src = strtok(cmd," "); //get source file name
        dest = strtok(NULL," "); //get dest file name


        sp = fopen(src, "rb"); //open source file
        dp = fopen(dest, "wb"); //open dest file
        if(!dp || !sp){
            perror("fopen");
            exit(1);
            }           
        int bytesRead;
        do{
            bytesRead = fread(buff,1, sizeof(buff), sp);
            if(bytesRead){
                fwrite(buff,1 , bytesRead, dp);
                bzero(buff, 2048);
            }

        }while(bytesRead);

        fclose(sp);
        fclose(dp);

    }

    else if(strncmp("DELETE ", cmd, 7) == 0){  // task k)  
        // this task use system calls - unlink
        cmd += 7;
        unlink(cmd);
    }    


    else{ // task i
        
        int fork_id = fork();

        if(fork_id == 0){// this is the child process
            
            char *word;
            char *args[strlen(cmd) + 1];
            int  i = 0;
            word = strtok(cmd, " ");
            while(word){
                args[i++] = word;
                word = strtok(NULL," ");                
            }

            args[i] = NULL;
            printf("%d", i);
            printf("%s f",args[0]);
            execvp(args[0], args);
        }

        else{ // this is the parent process, wait for the child to terminate
            wait(NULL);
        }

    }


}

void connectToServer(){

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    //create a socket and connect it to the tcp server
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(!sock) {
        perror("Opening socket failed");
        exit(1);
    }
    //build the server address
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    //connect to server
    if(connect(sock,(struct sockaddr*)&servaddr,sizeof(servaddr)) != 0){
        perror("Connecting failed");
        exit(1);
    }
    
}