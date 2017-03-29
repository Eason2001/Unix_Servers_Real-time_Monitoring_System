#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>   //socket, setsockop
#include <unistd.h>
#include <netinet/in.h>   //struct sockaddr_in
#include <netinet/tcp.h>  //TCP_NODELAY
#include <signal.h>
#include <errno.h>        //errno != EINTR
#include <string.h>
#include <sys/shm.h>  //for memory sharing
#include <arpa/inet.h> //inet_ntop
#include "extFunc.c"


extern void process_conn_server(int s);
extern int GetProfileString(char * profile, char * AppName, char * KeyName, char * KeyVal );
extern void monitor_workload_self();
extern void monitor_workload_other();
extern void monitor_workload_repto_master();

extern int SERVERSNUM;
extern struct serverPerform *sharedM;
extern char SERVERIP[30];
extern int SERVERPORT;
extern void init();

int main(int argc, char *argv[])
{

    int err;    //return value
    pid_t pid1;  

    //initiating the server
    init();  


    //fork the first process to monitor its own system performance
    pid1 = fork();       
    if ( pid1 < 0 )  
    {
       
        perror("fork process error");
        return -1;
    };
    if( pid1 == 0 )  
    {     
        monitor_workload_self();
      
        return 0;
    };
 


    if (MASTER_FLAG==1)   //it is a master server, listening on a port to provide service
    {
  
        int serSock,cliSock, tcpFlag;      
        struct sockaddr_in server_addr; 
        struct sockaddr_in client_addr;     
        char client_name[INET_ADDRSTRLEN];
        pid_t pid2;  

        printf("creating the socket for listening ...\n"); 
     
        //UPD:SOCK_DGRAM   TCP:SOCK_STREAM
        serSock = socket(AF_INET, SOCK_STREAM, 0);
        if(serSock < 0){
            perror("socket error");
            return -1;  
        };

        //optimizing the socket timeout 
        err = optimi_socket(serSock);
        if (err < 0) {
          perror("Couldn't optimize setting for serSock.)");
          return -1;
        };


        //initiating the server address struct 
        bzero(&server_addr, sizeof(server_addr));   
        server_addr.sin_family = AF_INET;           
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(SERVERPORT);         

        //binding the socket to the server address struct
        err = bind(serSock, (struct sockaddr*)&server_addr, sizeof(server_addr));
        if(err < 0){
            perror("bind error");
            return -1;
        }

        //listening on a port to provice service
        err = listen(serSock, BACKLOG);
        if(err < 0){
            perror("listen error");
            return -1;  
        }


        socklen_t addrlen = sizeof(client_addr);
        printf("the server is waiting on prot: %d \n", SERVERPORT); 

     
        for(;;) 
        {
            //waiting to see if there is any connection to connect to the server
            cliSock = accept(serSock, (struct sockaddr*)&client_addr, &addrlen);
            printf("got a Client: \n"); 

            //accetping error, continue to next loop to accept it again
            if(cliSock < 0){     
                close(cliSock); 
                continue;   
            };

            //optimizing the socket timeout 
            err = optimi_socket(cliSock);
            if (err < 0) {
              perror("Couldn't optimize setting for cliSock.)");
              return -1;
            };

            //testing the client address 
            if(inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,client_name,sizeof(client_name)) == NULL)
            {
                printf("Unable to get client address!\n",stderr);
                close(cliSock); 
                continue;  

            };


            //successfully accepting a client connection, then fork a new child process to handle it
            pid2 = fork();       
            if( pid2 == 0 )  
            {     
                close(serSock);      //close the listening socket in child process 
                process_conn_server(cliSock); //call the function to handle the request
                return 0;

                
            }else if ( pid2 > 0 )  //close the client socket and go back to listen on a port for next connection
            {

                close(cliSock);  
            }else 
            {
                 perror("fork process error");   
            }


        }

    }

    else{    //it is a slave server

        int cliSockSL;
        struct sockaddr_in ma_server_addrSL; 

        //it is a slave server, creating a connection to master server and sending the updating workload info to it
        printf("creating socket ... ... \n"); 
        cliSockSL = socket(AF_INET, SOCK_STREAM, 0);
        if(cliSockSL < 0){
            printf("socket error\n");
            return -1;  
        };

        //optimizing the socket timeout 
        err = optimi_socket(cliSockSL);
        if (err < 0) {
          printf("Couldn't optimize setting for cliSock.)\n");
          return -1;
        };


        //initiating the server address struct 
        bzero(&ma_server_addrSL, sizeof(ma_server_addrSL));       
        ma_server_addrSL.sin_family = AF_INET;               
        ma_server_addrSL.sin_addr.s_addr = htonl(INADDR_ANY);
        ma_server_addrSL.sin_port = htons(SERVERPORT);            

        //changing the string IP addr to a integer addr
        inet_pton(AF_INET, SERVERIP, &ma_server_addrSL.sin_addr); 
        //connecting to the master server
        printf("connecting to the master server:   %s: %d\n", SERVERIP, SERVERPORT); 
        connect(cliSockSL, (struct sockaddr*)&ma_server_addrSL, sizeof(struct sockaddr_in));

        //call the function to send workload info to the master server
        process_conn_clientSL(cliSockSL); 
        close(cliSockSL);   
        printf("exit\n");
        exit(0);

    }



}



