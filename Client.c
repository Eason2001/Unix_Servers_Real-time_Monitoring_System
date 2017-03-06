#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <arpa/inet.h>
#include "extFunc.c"

extern void sig_proccess(int signo);
extern void sig_pipe(int signo);
static int cliSock;


void sig_proccess_client(int signo)
{
    printf("Catch a exit signal\n");
    close(cliSock);
    exit(0);    
}

#define SERVERPORT 8888   /* 侦听端口地址 */
int main(int argc, char *argv[])
{

    struct sockaddr_in server_addr; /* 服务器地址结构 */
    int err;/* 返回值 */

    //设置程序运行环境，当程序接受到外界的SIGINT中断信号，
    signal(SIGINT, sig_proccess);
    signal(SIGPIPE, sig_pipe); //ignoring the signal: SIG_IGN

    /* 建立一个流式套接字 */
    cliSock = socket(AF_INET, SOCK_STREAM, 0);
    if(cliSock < 0){/* 出错 */
        printf("socket error\n");
        return -1;  
    };

    /* Disable the Nagle (TCP No Delay) algorithm */
    err = optimi_socket(cliSock);
    if (err < 0) {
      printf("Couldn't optimize setting for cliSock.)\n");
      return -1;
    };



    /* 设置服务器地址 */
    bzero(&server_addr, sizeof(server_addr));       /* 清0 */
    server_addr.sin_family = AF_INET;               /* 协议族 */
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);/* 本地地址 */
    server_addr.sin_port = htons(SERVERPORT);             /* 服务器端口 */

    /* 将用户输入的字符串类型的IP地址转为整型 */
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr); 
    /* 连接服务器 */
    connect(cliSock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
    // if(connect(cliSock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) <0 )
    // {
    //     perror("Client socket connect error");         
    //     return -1;
    // };
    

    process_conn_client(cliSock); /* 客户端处理过程 */
    close(cliSock);   /* 关闭连接 */
    printf("exit\n");
    exit(0);

}