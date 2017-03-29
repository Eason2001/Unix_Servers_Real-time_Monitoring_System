#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
//#include <sys/socket.h>   //socket, setsockop
#include <unistd.h>
//#include <netinet/in.h>  //struct sockaddr_in
//#include <netinet/tcp.h>  //TCP_NODELAY
#include <signal.h>
#include <errno.h>        //errno != EINTR
#include <string.h>
//#include <sys/shm.h>  //for memory sharing



int SERVERPORT=18888;       /* 侦听端口地址 */
int BACKLOG=2;       /* 侦听队列长度 */
int SERVERSNUM=10;       /* 侦听队列长度 */

struct serverPerform
{  
  int DISK_us, DISK_fr;  
  long int MEM_total, MEM_phy_F, MEM_cach_F, MEM_used;
  double totalWload, wload1m, wload5m, wload15m;
  double CPUload_us, CPUload_sy, CPUload_ni, CPUload_id, CPUload_wa;
  int phy_cpu_num, log_cpu_num, cpu_num;
  struct serverPerform * ptrNext; 

  
};

struct serverPerform *shared;


// void creShrMem(int signo)
// {
//     printf("Catch a exit signal\n");
//     exit(0);    
// }



char* shellcmd(char* cmd, char* buff, int size)
{
    char temp[256];
    FILE* fp = NULL;
    int offset = 0;
    int len;

    //using popen to excute shell cmd 
    fp = popen(cmd, "r");
    if(fp == NULL)
    {
    return NULL;
    }

    while(fgets(temp, sizeof(temp), fp) != NULL)
    {
    len = strlen(temp);
    if(offset + len < size)
    {
      strcpy(buff+offset, temp);
      offset += len;
    }
    else
    {
      buff[offset] = 0;
      break;
    }
    }

    if(fp != NULL)
    {
    pclose(fp);
    }

    return buff;
}




void monitor_workload()
{
      // char buff[30];
    int i=0;

    for(;;)
    {/* 循环处理过程 */
      bzero(shared, SERVERSNUM*sizeof(struct serverPerform));

      if (SERVERSNUM > 1)
      {

        for(i=0;i<SERVERSNUM;i++){


            shared[i].phy_cpu_num=1;
            shared[i].log_cpu_num=1;
            shared[i].cpu_num=1;
            shared[i].totalWload=1;
            shared[i].wload1m=1;
            shared[i].wload5m=1;
            shared[i].wload15m=1;
            shared[i].CPUload_us=1;
            shared[i].CPUload_sy=1;
            shared[i].CPUload_ni=1;
            shared[i].CPUload_id=1;
            shared[i].CPUload_wa=1;
            shared[i].MEM_total=1;
            shared[i].MEM_phy_F=1;
            shared[i].MEM_cach_F=1;
            shared[i].MEM_used=1;
            shared[i].DISK_us=1;
            shared[i].DISK_fr=1;

            // shared[i].phy_cpu_num=1;
            // shared[i].log_cpu_num=2;
            // shared[i].cpu_num=3;
            // shared[i].totalWload=1.000000;
            // shared[i].wload1m=2.000000;
            // shared[i].wload5m=3.000000;
            // shared[i].wload15m=4.000000;
            // shared[i].CPUload_us=5.000000;
            // shared[i].CPUload_sy=6.000000;
            // shared[i].CPUload_ni=7.000000;
            // shared[i].CPUload_id=8.000000;
            // shared[i].CPUload_wa=9.000000;
            // shared[i].MEM_total=1;
            // shared[i].MEM_phy_F=2;
            // shared[i].MEM_cach_F=3;
            // shared[i].MEM_used=4;
            // shared[i].DISK_us=4;
            // shared[i].DISK_fr=5;





            // printf("----------------------------------\n");
            // printf("------------Server %d------------\n", i);
            // printf("----------------------------------\n");
            // printf("phy_cpu_num: %d \n", shared[i].phy_cpu_num);
            // printf("log_cpu_num: %d \n", shared[i].log_cpu_num);
            // printf("cpu_num: %d \n", shared[i].cpu_num);
            // printf("totalWload: %f \n", shared[i].totalWload);
            // printf("wload1m: %f \n", shared[i].wload1m);
            // printf("wload5m: %f \n", shared[i].wload5m);
            // printf("wload15m: %f \n", shared[i].wload15m);
            // printf("CPUload_us: %f \n", shared[i].CPUload_us);
            // printf("CPUload_sy: %f \n", shared[i].CPUload_sy);
            // printf("CPUload_ni: %f \n", shared[i].CPUload_ni);
            // printf("CPUload_id: %f \n", shared[i].CPUload_id);
            // printf("CPUload_wa: %f \n", shared[i].CPUload_wa);
            // printf("MEM_total: %ld \n", shared[i].MEM_total);
            // printf("MEM_phy_F: %ld \n", shared[i].MEM_phy_F);
            // printf("MEM_cach_F: %ld \n", shared[i].MEM_cach_F);
            // printf("MEM_used: %ld \n", shared[i].MEM_used);
            // printf("DISK_us: %d \n", shared[i].DISK_us);
            // printf("DISK_fr: %d \n", shared[i].DISK_fr);
            // printf("----------------------------------\n");
            // printf("----------------------------------\n");



            


          }
      }

      else 
      {


            // printf("----------------------------------\n");
            // printf("------------Server %d------------\n", i);
            // printf("----------------------------------\n");
            // printf("phy_cpu_num: %d \n", shared->phy_cpu_num);
            // printf("log_cpu_num: %d \n", shared->log_cpu_num);
            // printf("cpu_num: %d \n", shared->cpu_num);
            // printf("totalWload: %f \n", shared->totalWload);
            // printf("wload1m: %f \n", shared->wload1m);
            // printf("wload5m: %f \n", shared->wload5m);
            // printf("wload15m: %f \n", shared->wload15m);
            // printf("CPUload_us: %f \n", shared->CPUload_us);
            // printf("CPUload_sy: %f \n", shared->CPUload_sy);
            // printf("CPUload_ni: %f \n", shared->CPUload_ni);
            // printf("CPUload_id: %f \n", shared->CPUload_id);
            // printf("CPUload_wa: %f \n", shared->CPUload_wa);
            // printf("MEM_total: %ld \n", shared->MEM_total);
            // printf("MEM_phy_F: %ld \n", shared->MEM_phy_F);
            // printf("MEM_cach_F: %ld \n", shared->MEM_cach_F);
            // printf("MEM_used: %ld \n", shared->MEM_used);
            // printf("DISK_us: %d \n", shared->DISK_us);
            // printf("DISK_fr: %d \n", shared->DISK_fr);
            // printf("----------------------------------\n");
            // printf("----------------------------------\n");  


      }

      sleep(10);

    }


}




/* optimizing socket */
int optimi_socket(int s)
{
    int err=0;

    struct timeval timeo = {5, 0};  //second, usecond
    //发送时限  
    err = setsockopt(s,SOL_SOCKET,SO_SNDTIMEO,&timeo,sizeof(timeo));  
    if (err < 0) {
      perror("Couldn't setsockopt(Send NetTimeout)");

      return -1;
    };

    //接收时限  
    err = setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,&timeo,sizeof(timeo));  
    if (err < 0) {
      perror("Couldn't setsockopt(Receive NetTimeout)");
      return -1;
    };


    return 0;
}



/* 服务器对客户端的处理 */
void process_conn_server(int s)
{
    ssize_t size = 0;
    char sndBuffer[2048];  /* 数据的发送缓冲区 */
    char revBuffer[2048];  /* 数据的接收缓冲区 */    

    usleep(100000); 
    for(;;){/* 循环处理过程 */

        bzero(sndBuffer, sizeof(sndBuffer));   /* 清0 */
        bzero(revBuffer, sizeof(revBuffer));   /* 清0 */      
        /* 从套接字中读取数据放到缓冲区buffer中 */
        size = read(s, revBuffer, 2048);

        if (size == -1)
        {

            usleep(100000); 
            continue;

        };

        //Client close socket
        if (size == 0)
        {/* 没有数据 */ 
            // usleep(100000);
            close(s);
            printf("client exit\n");
            break; 
        };


        //client exit
        if (strncmp("exit", revBuffer, 4) == 0) 
        {             
            close(s); 
            printf("client exit\n");  
            break;   
        };

        printf("Client message: %s \n", revBuffer); 


        /* 构建响应字符，为接收到客户端字节的数量 */
        printf("It is going to write data below to the client: \n"); 

        memcpy(sndBuffer,shared,SERVERSNUM*sizeof(struct serverPerform));
        printf("the length of sending buffer is: %ld \n", sizeof(sndBuffer)+1);
        write(s, sndBuffer, SERVERSNUM*sizeof(struct serverPerform)+1);
        sleep(1);

    }   



}

/* 客户端的处理过程 */
void process_conn_client(int s)
{
    ssize_t size = 0;
    char sndBuffer[1024];  /* 数据的发送缓冲区 */
    char revBuffer[1024];  /* 数据的接收缓冲区 */    
    struct serverPerform *revSerInfo;
    int i;
    
    for(;;){/* 循环处理过程 */
        /* 从标准输入中读取数据放到缓冲区buffer中 */

        bzero(sndBuffer, sizeof(sndBuffer));   /* 清0 */
        bzero(revBuffer, sizeof(revBuffer));   /* 清0 */ 

        size = read(0, sndBuffer, 1024);
        if(size == 0)
        {/* 没有读到数据 */
            perror("Read end user input error\n"); 
            break; //read eof 
        };
        write(s, sndBuffer, size);     /* 发送给服务器 */
        usleep(100000);  




        size = read(s, revBuffer, 1024);/* 从服务器读取数据 */
        if (size < 0)
        {
          /* code */
            perror(" reading socket error ");

        };

        if(size == 0){/* 服务器端关闭，清空了缓冲区数据 */
            perror("Server terminated prematurely\n");
            // usleep(500000);
            break;

        };

        revSerInfo=(struct serverPerform *)malloc(SERVERSNUM*sizeof(struct serverPerform));

        memcpy(revSerInfo, revBuffer, SERVERSNUM*sizeof(struct serverPerform));
        printf("The info from server is as below: \n");

        if (SERVERSNUM > 1)
        {

          for(i=0;i<SERVERSNUM;i++){

   
              printf("----------------------------------\n");
              printf("------------Server %d-------------\n", i);
              printf("----------------------------------\n");
              printf("phy_cpu_num: %d \n", revSerInfo[i].phy_cpu_num);
              printf("log_cpu_num: %d \n", revSerInfo[i].log_cpu_num);
              printf("cpu_num: %d \n", revSerInfo[i].cpu_num);
              printf("totalWload: %f \n", revSerInfo[i].totalWload);
              printf("wload1m: %f \n", revSerInfo[i].wload1m);
              printf("wload5m: %f \n", revSerInfo[i].wload5m);
              printf("wload15m: %f \n", revSerInfo[i].wload15m);
              printf("CPUload_us: %f \n", revSerInfo[i].CPUload_us);
              printf("CPUload_sy: %f \n", revSerInfo[i].CPUload_sy);
              printf("CPUload_ni: %f \n", revSerInfo[i].CPUload_ni);
              printf("CPUload_id: %f \n", revSerInfo[i].CPUload_id);
              printf("CPUload_wa: %f \n", revSerInfo[i].CPUload_wa);
              printf("MEM_total: %ld \n", revSerInfo[i].MEM_total);
              printf("MEM_phy_F: %ld \n", revSerInfo[i].MEM_phy_F);
              printf("MEM_cach_F: %ld \n", revSerInfo[i].MEM_cach_F);
              printf("MEM_used: %ld \n", revSerInfo[i].MEM_used);
              printf("DISK_us: %d \n", revSerInfo[i].DISK_us);
              printf("DISK_fr: %d \n", revSerInfo[i].DISK_fr);
              printf("----------------------------------\n");
              printf("----------------------------------\n");

            }
        }

        else 
        {

              // printf("----------------------------------\n");
              // printf("------------Server %d-------------\n", i);
              // printf("----------------------------------\n");
              // printf("phy_cpu_num: %d \n", shared->phy_cpu_num);
              // printf("log_cpu_num: %d \n", shared->log_cpu_num);
              // printf("cpu_num: %d \n", shared->cpu_num);
              // printf("totalWload: %f \n", shared->totalWload);
              // printf("wload1m: %f \n", shared->wload1m);
              // printf("wload5m: %f \n", shared->wload5m);
              // printf("wload15m: %f \n", shared->wload15m);
              // printf("CPUload_us: %f \n", shared->CPUload_us);
              // printf("CPUload_sy: %f \n", shared->CPUload_sy);
              // printf("CPUload_ni: %f \n", shared->CPUload_ni);
              // printf("CPUload_id: %f \n", shared->CPUload_id);
              // printf("CPUload_wa: %f \n", shared->CPUload_wa);
              // printf("MEM_total: %ld \n", shared->MEM_total);
              // printf("MEM_phy_F: %ld \n", shared->MEM_phy_F);
              // printf("MEM_cach_F: %ld \n", shared->MEM_cach_F);
              // printf("MEM_used: %ld \n", shared->MEM_used);
              // printf("DISK_us: %d \n", shared->DISK_us);
              // printf("DISK_fr: %d \n", shared->DISK_fr);
              // printf("----------------------------------\n");
              // printf("----------------------------------\n");  


        };

        bzero(revSerInfo, SERVERSNUM*sizeof(struct serverPerform));


    }


}

//SIGINT       terminate process    interrupt program
void sig_proccess(int signo)
{
    printf("Catch a exit signal\n");
    exit(0);
}

//SIGPIPE      terminate process    write on a pipe with no reader
void sig_pipe(int sign)
{
    printf("Catch a SIGPIPE signal\n");

    /* 释放资源 */  
}
