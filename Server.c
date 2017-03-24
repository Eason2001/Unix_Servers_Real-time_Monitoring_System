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

    int err;    /* 返回值 */
    pid_t pid1;  /* 分叉的进行id */


    init();  


    //fork the first process to monitor its own system performance
    pid1 = fork();       /* 分叉进程 */
    if ( pid1 < 0 )  /* 在父进程中关闭客户端的连接 */
    {
        /* code */
        perror("fork process error");
        return -1;
    };
    if( pid1 == 0 )  /* 第一个子进程中:自我监控 */
    {     
        monitor_workload_self();
      
        return 0;
    };
 


    if (MASTER_FLAG==1)   //it is a master server
    {
        /* code */
        int serSock,cliSock, tcpFlag;      /* serSock为服务器的socket描述符,cliSock为客户端的socket描述符 */
        struct sockaddr_in server_addr; /* 服务器地址结构 */
        struct sockaddr_in client_addr; /* 客户端地址结构 */    
        char client_name[INET_ADDRSTRLEN];
        pid_t pid2;  /* 分叉的进行id */

        printf("forking the second process for listening ...\n"); 
        /* 建立一个流式套接字 */
        //UPD:SOCK_DGRAM   TCP:SOCK_STREAM
        serSock = socket(AF_INET, SOCK_STREAM, 0);
        if(serSock < 0){/* 出错 */
            perror("socket error");
            return -1;  
        };

        /* Disable the Nagle (TCP No Delay) algorithm */
        err = optimi_socket(serSock);
        if (err < 0) {
          perror("Couldn't optimize setting for serSock.)");
          return -1;
        };


        /* 设置服务器地址 */
        bzero(&server_addr, sizeof(server_addr));   /* 清0 */
        server_addr.sin_family = AF_INET;           /* 协议族 */
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);/* 本地地址 */
        server_addr.sin_port = htons(SERVERPORT);         /* 服务器端口 */

        /* 绑定地址结构到套接字描述符 */
        err = bind(serSock, (struct sockaddr*)&server_addr, sizeof(server_addr));
        if(err < 0){/* 出错 */
            perror("bind error");
            return -1;
        }

        /* 设置侦听 */
        err = listen(serSock, BACKLOG);
        if(err < 0){/* 出错 */
            perror("listen error");
            return -1;  
        }



        socklen_t addrlen = sizeof(client_addr);
        printf("the server is waiting on prot: %d \n", SERVERPORT); 

        /* 主循环过程 */
        for(;;) 
        {


            /* 接收客户端连接 */
            cliSock = accept(serSock, (struct sockaddr*)&client_addr, &addrlen);
            printf("got a Client: \n"); 

            if(cliSock < 0){     /* 出错 */
                close(cliSock); 
                continue;   /* 结束本次循环 */
            };

            err = optimi_socket(cliSock);
            if (err < 0) {
              perror("Couldn't optimize setting for cliSock.)");
              return -1;
            };


            if(inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,client_name,sizeof(client_name)) == NULL)
            {
                printf("Unable to get client address!\n",stderr);
                close(cliSock); 
                continue;   /* 结束本次循环 */

            };


            /* 建立一个新的进程处理到来的连接 */
            pid2 = fork();       /* 分叉进程 */
            if( pid2 == 0 )  /* 第二个子进程中 */
            {     
                close(serSock);      /* 在子进程中关闭服务器的侦听 */
                process_conn_server(cliSock);/* 处理连接 */
                return 0;

                
            }else if ( pid2 > 0 )  /* 在父进程中关闭客户端的连接 */
            {
                /* code */
                close(cliSock);  
            }else 
            {
                 perror("fork process error");   
            }


        }

    }

    else{    //it is a slave server

        int cliSockSL;
        struct sockaddr_in ma_server_addrSL; /* 服务器地址结构 */

        printf("creating socket ... ... \n"); 
        cliSockSL = socket(AF_INET, SOCK_STREAM, 0);
        if(cliSockSL < 0){/* 出错 */
            printf("socket error\n");
            return -1;  
        };

        /* Disable the Nagle (TCP No Delay) algorithm */
        err = optimi_socket(cliSockSL);
        if (err < 0) {
          printf("Couldn't optimize setting for cliSock.)\n");
          return -1;
        };


        /* 设置服务器地址 */
        bzero(&ma_server_addrSL, sizeof(ma_server_addrSL));       /* 清0 */
        ma_server_addrSL.sin_family = AF_INET;               /* 协议族 */
        ma_server_addrSL.sin_addr.s_addr = htonl(INADDR_ANY);/* 本地地址 */
        ma_server_addrSL.sin_port = htons(SERVERPORT);             /* 服务器端口 */

        /* 将用户输入的字符串类型的IP地址转为整型 */
        inet_pton(AF_INET, SERVERIP, &ma_server_addrSL.sin_addr); 
        /* 连接服务器 */
        printf("connecting to the master server:   %s: %d\n", SERVERIP, SERVERPORT); 
        connect(cliSockSL, (struct sockaddr*)&ma_server_addrSL, sizeof(struct sockaddr_in));

        process_conn_clientSL(cliSockSL); /* 客户端处理过程 */
        free(sharedM);
        close(cliSockSL);   /* 关闭连接 */
        printf("exit\n");
        exit(0);

    }



}



