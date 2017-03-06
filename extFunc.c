#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <netinet/tcp.h>  //TCP_NODELAY
#include <sys/types.h>
#include <sys/socket.h>   //socket, setsockop, SOL_SOCKET
#include <netinet/in.h>   //struct sockaddr_in
#include <errno.h>        //errno != EINTR
#include <strings.h>  


struct serverPerform
{
  int phy_cpu_num, log_cpu_num, cpu_num;
  double totalWload, wload1m, wload5m, wload15m;
  double CPUload_us, CPUload_sy, CPUload_ni, CPUload_id, CPUload_wa;
  long int MEM_total, MEM_phy_F, MEM_cach_F, MEM_used;
  int DISK_us, DISK_fr;
  struct serverPerform * ptrNext; 
  
};

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
  char buff[30];

  struct serverPerform maServer;



    for(;;){/* 循环处理过程 */



      memset(buff, 0, sizeof(buff));
      bzero(&maServer, sizeof(maServer));

      maServer.phy_cpu_num=atoi(shellcmd("cat /proc/cpuinfo| grep \"physical id\"| sort| uniq| wc -l", buff, sizeof(buff)));
      maServer.log_cpu_num=atoi(shellcmd("cat /proc/cpuinfo | grep \"siblings\"| sort -n | tail -1 |awk '{print $3}'", buff, sizeof(buff)));
      maServer.cpu_num=maServer.phy_cpu_num*maServer.log_cpu_num;
      maServer.totalWload=maServer.cpu_num*1.00;
      maServer.wload1m=atof(shellcmd("echo \"scale=4;tmp1=$(uptime | awk '{print $10}'|cut -f 1 -d \",\");if(length(tmp1)==scale(tmp1)) print 0;print tmp1\" | bc", buff, sizeof(buff)))/maServer.totalWload;
      maServer.wload5m=atof(shellcmd("echo \"scale=4;tmp2=$(uptime | awk '{print $11}'|cut -f 1 -d \",\");if(length(tmp2)==scale(tmp2)) print 0;print tmp2\"| bc", buff, sizeof(buff)))/maServer.totalWload;
      maServer.wload15m=atof(shellcmd("echo \"scale=4;tmp3=$(uptime | awk '{print $NF}'|cut -f 1 -d \",\");if(length(tmp3)==scale(tmp3)) print 0;print tmp3\"| bc", buff, sizeof(buff)))/maServer.totalWload;
      maServer.CPUload_us=atof(shellcmd("echo \"tmp=$(top -b -n 1 | grep Cpu | awk '{print $2}');if(length(tmp)==scale(tmp)) print 0;print tmp\" |bc", buff, sizeof(buff)));
      maServer.CPUload_sy=atof(shellcmd("echo \"tmp=$(top -b -n 1 | grep Cpu | awk '{print $4}');if(length(tmp)==scale(tmp)) print 0;print tmp\" |bc", buff, sizeof(buff)));
      maServer.CPUload_ni=atof(shellcmd("echo \"tmp=$(top -b -n 1 | grep Cpu | awk '{print $6}');if(length(tmp)==scale(tmp)) print 0;print tmp\" |bc", buff, sizeof(buff)));
      maServer.CPUload_id=atof(shellcmd("echo \"tmp=$(top -b -n 1 | grep Cpu | awk '{print $8}');if(length(tmp)==scale(tmp)) print 0;print tmp\" |bc", buff, sizeof(buff)));
      maServer.CPUload_wa=atof(shellcmd("echo \"tmp=$(top -b -n 1 | grep Cpu | awk '{print $10}');if(length(tmp)==scale(tmp)) print 0;print tmp\" |bc", buff, sizeof(buff)));
      maServer.MEM_total=atol(shellcmd("free -m | grep Mem | awk '{print $2}'", buff, sizeof(buff)));
      maServer.MEM_phy_F=atol(shellcmd("free -m | grep Mem | awk '{print $4}'", buff, sizeof(buff)));
      maServer.MEM_cach_F=atol(shellcmd("free -m | grep - | awk '{print $NF}'", buff, sizeof(buff)))-maServer.MEM_phy_F;
      maServer.MEM_used=atol(shellcmd("free -m | grep - | awk '{print $3}'", buff, sizeof(buff)));
      maServer.DISK_us=atoi(shellcmd("df -h | awk '{print $5}' | grep % | grep -v Use | sort -n | tail -1 | cut -d \"%\" -f1 -", buff, sizeof(buff)));
      maServer.DISK_fr=100-maServer.DISK_us;

      printf("----------------------------------\n");
      printf("----------------------------------\n");
      printf("phy_cpu_num: %d \n", maServer.phy_cpu_num);
      printf("log_cpu_num: %d \n", maServer.log_cpu_num);
      printf("cpu_num: %d \n", maServer.cpu_num);
      printf("totalWload: %f \n", maServer.totalWload);
      printf("wload1m: %f \n", maServer.wload1m);
      printf("wload5m: %f \n", maServer.wload5m);
      printf("wload15m: %f \n", maServer.wload15m);
      printf("CPUload_us: %f \n", maServer.CPUload_us);
      printf("CPUload_sy: %f \n", maServer.CPUload_sy);
      printf("CPUload_ni: %f \n", maServer.CPUload_ni);
      printf("CPUload_id: %f \n", maServer.CPUload_id);
      printf("CPUload_wa: %f \n", maServer.CPUload_wa);
      printf("MEM_total: %ld \n", maServer.MEM_total);
      printf("MEM_phy_F: %ld \n", maServer.MEM_phy_F);
      printf("MEM_cach_F: %ld \n", maServer.MEM_cach_F);
      printf("MEM_used: %ld \n", maServer.MEM_used);
      printf("DISK_us: %d \n", maServer.DISK_us);
      printf("DISK_fr: %d \n", maServer.DISK_fr);
      sleep(5);




    }



}




/* optimizing socket */
int optimi_socket(int s)
{
    int err=0;


    /* Disable the Nagle (TCP No Delay) algorithm */
    // int tcpFlag = 1;
    // err = setsockopt(s,IPPROTO_TCP,TCP_NODELAY,(char *)&tcpFlag,sizeof(tcpFlag));
    // if (err < 0) {
    //   // printf("Couldn't setsockopt(TCP_NODELAY)\n");
    //   perror("Couldn't setsockopt(TCP_NODELAY)");
    //   return -1;
    // };

    // struct timeval {
    // time_t      tv_sec;    
    // suseconds_t tv_usec;   
    // }; 
    
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


    // //如果在发送数据的时，希望不经历由系统缓冲区到socket缓冲区的拷贝而影响程序的性能：
    // int nZero=10*1024;  
    // err = setsockopt(s,SOL_SOCKET,SO_SNDBUF,(char *)&nZero,sizeof(nZero));  
    // if (err < 0) {
    //   perror("Couldn't setsockopt(Send buffer)");

    //   return -1;
    // };    
    // //同上在recv()完成上述功能(默认情况是将socket缓冲区的内容拷贝到系统缓冲区)：
    // err = setsockopt(s,SOL_SOCKET,SO_RCVBUF,(char *)&nZero,sizeof(nZero));  
    // if (err < 0) {
    //   perror("Couldn't setsockopt(Receive buffer)");
    //   return -1;
    // };

    return 0;
}



/* 服务器对客户端的处理 */
void process_conn_server(int s)
{
    ssize_t size = 0;
    char sndBuffer[1024];  /* 数据的发送缓冲区 */
    char revBuffer[1024];  /* 数据的接收缓冲区 */    

    usleep(500000); 
    for(;;){/* 循环处理过程 */

        bzero(sndBuffer, sizeof(sndBuffer));   /* 清0 */
        bzero(revBuffer, sizeof(revBuffer));   /* 清0 */      
        /* 从套接字中读取数据放到缓冲区buffer中 */
        size = read(s, revBuffer, 1024);

        if (size < 0)
        {
            // if (errno != EINTR)
            // {
            //     /* code */
            //     perror("Server socket read error");
            //     break;

            // };

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


        /* 构建响应字符，为接收到客户端字节的数量 */


        sprintf(sndBuffer, "%zd bytes altogether!\n", size);
        write(s, sndBuffer, strlen(sndBuffer)+1);/* 发给客户端 */
        // usleep(100000);

    }   
}

/* 客户端的处理过程 */
void process_conn_client(int s)
{
    ssize_t size = 0;
    char sndBuffer[1024];  /* 数据的发送缓冲区 */
    char revBuffer[1024];  /* 数据的接收缓冲区 */    


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
        usleep(300000);        
        size = read(s, revBuffer, 1024);/* 从服务器读取数据 */

        if(size == 0){/* 服务器端关闭，清空了缓冲区数据 */
            perror("Server terminated prematurely\n");
            // usleep(500000);
            break;

        }

        write(1, revBuffer, size);     /* 写到标准输出 */
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