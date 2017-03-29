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


void sig_proccess(int signo);
void sig_pipe(int signo);
int optimi_socket(int s);
void process_conn_clientTS(int s);

int SERVERSNUM=3;

static int cliSock;


struct serverPerform
{
  int DISK_us, DISK_fr;
  long int MEM_total, MEM_phy_F, MEM_cach_F, MEM_used;   
  double totalWload, wload1m, wload5m, wload15m;
  double CPUload_us, CPUload_sy, CPUload_ni, CPUload_id, CPUload_wa;
  int phy_cpu_num, log_cpu_num, cpu_num;
  int slave_index;
  struct serverPerform * ptrNext; 
};


struct serverPerform climaServer;



int main(int argc, char *argv[])
{

    struct sockaddr_in server_addr; 
    int err;

    if(argc != 3)
    {
        printf("Please refer to the usage below: \n");
        printf("usage: %s  <ServerIP> <ServerPORT>\n", argv[0]);
        return 0;

    };


    
    signal(SIGINT, sig_proccess);    //sig_proccess
    signal(SIGPIPE, SIG_IGN);   //sig_pipe
    signal(SIGHUP, SIG_IGN);   //sig_proccess
    signal(SIGQUIT, SIG_IGN);   //sig_proccess
    //ignoring the signal: SIG_IGN



   
    cliSock = socket(AF_INET, SOCK_STREAM, 0);
    if(cliSock < 0){
        printf("socket error\n");
        return -1;  
    };


    err = optimi_socket(cliSock);
    if (err < 0) {
      printf("Couldn't optimize setting for cliSock.)\n");
      return -1;
    };


   
    bzero(&server_addr, sizeof(server_addr));       
    server_addr.sin_family = AF_INET;               
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[2]));             

    
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr); 
    connect(cliSock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));

    

    process_conn_clientTS(cliSock); 
    close(cliSock);   
    printf("exit\n");
    exit(0);


}

void process_conn_clientTS(int s)
{
    ssize_t size = 0;
    char sndBuffer[1024]; 
    char revBuffer[1024];  
    struct serverPerform *revSerInfo;
    int i;
    
    for(;;){
       
        sleep(2);
        bzero(sndBuffer, sizeof(sndBuffer));   
        bzero(revBuffer, sizeof(revBuffer));  

        strcpy(sndBuffer,"Android_GET"); 
        write(s, sndBuffer, strlen(sndBuffer));     

        size = read(s, revBuffer, 1024);
        if (size < 0)
        {

            perror(" reading socket error ");

        };

        if(size == 0){
            perror("Server terminated prematurely\n");

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
              printf("slave_index: %d \n", revSerInfo[i].slave_index);
              printf("----------------------------------\n");
              printf("----------------------------------\n");

            }
        }

        else 
        {

              // printf("----------------------------------\n");
              // printf("------------Server %d-------------\n", i);
              // printf("----------------------------------\n");
              // printf("phy_cpu_num: %d \n", sharedM->phy_cpu_num);
              // printf("log_cpu_num: %d \n", sharedM->log_cpu_num);
              // printf("cpu_num: %d \n", sharedM->cpu_num);
              // printf("totalWload: %f \n", sharedM->totalWload);
              // printf("wload1m: %f \n", sharedM->wload1m);
              // printf("wload5m: %f \n", sharedM->wload5m);
              // printf("wload15m: %f \n", sharedM->wload15m);
              // printf("CPUload_us: %f \n", sharedM->CPUload_us);
              // printf("CPUload_sy: %f \n", sharedM->CPUload_sy);
              // printf("CPUload_ni: %f \n", sharedM->CPUload_ni);
              // printf("CPUload_id: %f \n", sharedM->CPUload_id);
              // printf("CPUload_wa: %f \n", sharedM->CPUload_wa);
              // printf("MEM_total: %ld \n", sharedM->MEM_total);
              // printf("MEM_phy_F: %ld \n", sharedM->MEM_phy_F);
              // printf("MEM_cach_F: %ld \n", sharedM->MEM_cach_F);
              // printf("MEM_used: %ld \n", sharedM->MEM_used);
              // printf("DISK_us: %d \n", sharedM->DISK_us);
              // printf("DISK_fr: %d \n", sharedM->DISK_fr);
              // printf("----------------------------------\n");
              // printf("----------------------------------\n");  


        };

        bzero(revSerInfo, SERVERSNUM*sizeof(struct serverPerform));


    }


}


//interrup signal handling function
void sig_proccess(int signo)
{
    printf("Catch a exit signal!\n");
    kill(0,SIGTERM);
    printf("the program will exit\n");
    exit(1);

}


//pipe signal handling function
void sig_pipe(int sign)
{
    //can be different from sig_process according to different requirement
    printf("Catch a SIGPIPE signal!\n");
    kill(0,SIGTERM);
    printf("the program will exit\n");
    exit(1);
    
}

/* optimizing socket */
int optimi_socket(int s)
{
    int err=0;

    //not using this setting: for TCP No Delay
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
    //sending timeout 
    err = setsockopt(s,SOL_SOCKET,SO_SNDTIMEO,&timeo,sizeof(timeo));  
    if (err < 0) {
      perror("Couldn't setsockopt(Send NetTimeout)");

      return -1;
    };

    //receiving timeout 
    err = setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,&timeo,sizeof(timeo));  
    if (err < 0) {
      perror("Couldn't setsockopt(Receive NetTimeout)");
      return -1;
    };


    //not using this setting: for sending directly without using buffer cache
    // int nZero=10*1024;  
    // err = setsockopt(s,SOL_SOCKET,SO_SNDBUF,(char *)&nZero,sizeof(nZero));  
    // if (err < 0) {
    //   perror("Couldn't setsockopt(Send buffer)");

    //   return -1;
    // };    
    // err = setsockopt(s,SOL_SOCKET,SO_RCVBUF,(char *)&nZero,sizeof(nZero));  
    // if (err < 0) {
    //   perror("Couldn't setsockopt(Receive buffer)");
    //   return -1;
    // };

    return 0;
}



