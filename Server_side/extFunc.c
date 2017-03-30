#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>   //socket, setsockop
#include <unistd.h>
#include <netinet/in.h>  //struct sockaddr_in
#include <netinet/tcp.h>  //TCP_NODELAY
#include <signal.h>
#include <errno.h>        //errno != EINTR
#include <string.h>
#include <sys/shm.h>  //for memory sharing


char SERVERIP[30]={0};  //Server IP addr
int SERVERPORT=18888;   //Server port
int SERVERSNUM=2;       // the number of servers in this cluster
int MASTER_FLAG=0;      // label this server to be a master server or a slave server
int SLAVE_INDEX=1;      // the index of this slave server if it is a slave server
int BACKLOG=2;          // using for TCP listening sequence
int shmid;              // sharing memory ID
char Twitter_Api_Key[50]={0};   // used to send Twitter Message

#define cfgFile "Server.config" // name of configuration file 
#define CONLINELENGTH 100       // the maximum number of character of a line in configuration file

// function for read configuration items from a configuration file
int GetProfileString(char * profile, char * AppName, char * KeyName, char * KeyVal ); 
void init(); //used for initiating the server, including loading configuration, setting signal process, creating shared memory
// function for running a shell command, especially for obtaiing the server workload info and sending twitter message                                       // function for initiating a server
char* shellcmd(char* cmd, char* buff, int size);    
void monitor_workload_self();       // function for monitoring server's own workload information
int optimi_socket(int s);           // function for optimizing the timeout of TCP connection
void process_conn_server(int s);    // a master server to handle the request connection
void process_conn_clientSL(int s);  // a slave server to send request to a master for updating info 
void process_conn_clientTS(int s);  // android testing client to send request to a master for servers info
void sig_proccess(int signo);       // function for handling the signal
void sig_pipe(int sign);             // function for handling the signal


//global struct for recording the server info
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

//global struct pointer for pointing the sharing memory, so that all process forked from main process could share and
//access the sharing memory that was used to store the server workload info
struct serverPerform *sharedM;



int GetProfileString(char * profile, char * AppName, char * KeyName, char * KeyVal )
{
    char aname[30],kname[30];
    char buf[CONLINELENGTH],*c;
    FILE *fp;
    int found=0;
    if( (fp=fopen( profile,"r" ))==NULL )
    {
        printf( "openfile [%s] error [%s]\n",
        profile,strerror(errno));
        return(-1);
    };
    fseek( fp, 0, SEEK_SET );
    sprintf( aname,"%s", AppName );
    memset( kname, 0, sizeof(kname) );
    while( !feof(fp) && fgets( buf,CONLINELENGTH,fp )!=NULL )
    {

        if( found==0 )
        {
            if( buf[0]!='[' )
            {
                continue;
            }
            else if ( strncmp(buf,aname,strlen(aname))==0 )
            {
                found=1;
                continue;
            }
        }
        else if( found==1 )
        {
            if( buf[0]=='#' )
            {
                continue;
            }
        else if ( buf[0]=='[' )
        {
            break;
        }
        else
        {
            if( (c=(char*)strchr(buf,'='))==NULL )
                continue;
            memset( kname, 0, sizeof(kname) );
            sscanf( buf, "%[^=]", kname );
            if( strcmp(kname, KeyName)==0 )
                {
                    sscanf( ++c, "%[^\r]", KeyVal);
                    found=2;
                    break;
                }
            else
                {
                    continue;
                }
            }
        }
    }
    fclose( fp );
    if( found==2 )
    return(0);
    else
    return(-1);

}




void init()
{
    int retInt;
    void *shm = NULL;  


    char tmpStr1[30]={0}; //used to load parameters from Server.config

    printf("Initializing the server ... ...\n");

    printf("Setting the signal process\n");
    //seting signal process 
    signal(SIGINT, sig_proccess);    //sig_proccess
    signal(SIGPIPE, SIG_IGN);   //sig_pipe
    signal(SIGHUP, SIG_IGN);   //sig_proccess
    signal(SIGQUIT, SIG_IGN);   //sig_proccess
    //ignoring the signal: SIG_IGN

    //loading the configuration from configuration file
    printf("loading parameters from file: %s \n",cfgFile);

    memset(tmpStr1, 0, sizeof(char)*30);
    if(GetProfileString(cfgFile,"[Local_Server.config]","Master_Flag",tmpStr1))
    {
        perror("loading parameters from Server.config error");
        exit(1);
    };
    if(strlen(tmpStr1)!=0)
    MASTER_FLAG = atoi(tmpStr1);

    memset(tmpStr1, 0, sizeof(char)*30);
    if(GetProfileString(cfgFile,"[Local_Server.config]","Slave_Index",tmpStr1))
    {
        perror("loading parameters from Server.config error");
        exit(1);
    };
    if(strlen(tmpStr1)!=0)
    SLAVE_INDEX = atoi(tmpStr1);

    memset(tmpStr1, 0, sizeof(char)*30);
    if(GetProfileString(cfgFile,"[Twitter.config]","Twitter_Api_Key",tmpStr1))
    {
        perror("loading parameters from Server.config error");
        exit(1);
    };
    if(strlen(tmpStr1)!=0)
        memcpy(Twitter_Api_Key, tmpStr1, strlen(tmpStr1)-1);  //remove the enter key at the end of string



    if ( MASTER_FLAG==1 )  //local server is a master server
    {

        printf("This is a master server: \n");
        printf("the MASTER_FLAG: %d \n",MASTER_FLAG);
        SLAVE_INDEX=0;
        printf("the SLAVE_INDEX: %d \n", SLAVE_INDEX);

        memset(tmpStr1, 0, sizeof(char)*30);
        if(GetProfileString(cfgFile,"[Local_Server.config]","Local_Server_IP",tmpStr1))
        {
            perror("loading parameters from Server.config error");
            exit(1);
        };
        if(strlen(tmpStr1)!=0)
            strcpy(SERVERIP,tmpStr1);
        printf("The server IP: %s \n", SERVERIP);

        memset(tmpStr1, 0, sizeof(char)*30);
        if(GetProfileString(cfgFile,"[Local_Server.config]","Local_Server_Port",tmpStr1))
        {
            perror("loading parameters from Server.config error");
            exit(1);
        };
        if(strlen(tmpStr1)!=0)
        SERVERPORT = atoi(tmpStr1);
        printf("The service port: %d \n", SERVERPORT);

        memset(tmpStr1, 0, sizeof(char)*30);
        if(GetProfileString(cfgFile,"[Cluster_Server.config]","Number_Of_Servers",tmpStr1))
        {
            perror("loading parameters from Server.config error");
            exit(1);
        };
        if(strlen(tmpStr1)!=0)
        SERVERSNUM = atoi(tmpStr1);
        printf("The number of servers in cluster: %d \n", SERVERSNUM);

        //It is a master server, multiple struct variable will be created
        printf("Staring to create memory block shared among process...\n");  
        shmid = shmget((key_t)1234, SERVERSNUM*sizeof(struct serverPerform), 0666|IPC_CREAT);
        if(shmid == -1)  
        {  
            perror("shmget failed ");  
            exit(1);
        }  
          
        shm = shmat(shmid, 0, 0);  
        if(shm == (void*)-1)  
        {   
            perror("shmat failed ");  
            exit(1);
        };
        printf("the shared memory block was created successfully\n");  
        printf("Memory attached at %X\n", (int)shm);  
       
        sharedM = (struct serverPerform*)shm;  
        bzero(sharedM, SERVERSNUM*sizeof(struct serverPerform));


    }
    else  //local server a slave server
    {
        MASTER_FLAG=0;
        printf("This is a slave server: \n");
        printf("the MASTER_FLAG: %d \n",MASTER_FLAG);
        printf("the SLAVE_INDEX: %d \n", SLAVE_INDEX);

        memset(tmpStr1, 0, sizeof(char)*30);
        if(GetProfileString(cfgFile,"[Local_Server.config]","Local_Server_IP",tmpStr1))
        {
            perror("loading parameters from Server.config error");
            exit(1);
        };
        if(strlen(tmpStr1)!=0)
        printf("My local IP: %s \n", tmpStr1);


        memset(tmpStr1, 0, sizeof(char)*30);
        if(GetProfileString(cfgFile,"[Cluster_Server.config]","Master_Server_IP",tmpStr1))
        {
            perror("loading parameters from Server.config error");
            exit(1);
        };
        if(strlen(tmpStr1)!=0)
            strcpy(SERVERIP,tmpStr1);
        printf("My master server IP: %s \n", SERVERIP);

        memset(tmpStr1, 0, sizeof(char)*30);
        if(GetProfileString(cfgFile,"[Cluster_Server.config]","Master_Server_Port",tmpStr1))
        {
            perror("loading parameters from Server.config error");
            exit(1);
        };
        if(strlen(tmpStr1)!=0)
            SERVERPORT = atoi(tmpStr1);
        printf("My master server port: %d \n", SERVERPORT);

        memset(tmpStr1, 0, sizeof(char)*30);
        if(GetProfileString(cfgFile,"[Cluster_Server.config]","Number_Of_Servers",tmpStr1))
        {
            perror("loading parameters from Server.config error");
            exit(1);
        };
        if(strlen(tmpStr1)!=0)
        SERVERSNUM = atoi(tmpStr1);
        printf("The number of servers in cluster: %d \n", SERVERSNUM);

        //It is a slave server, single struct variable will be created
        printf("Staring to create memory block shared among process...\n");  
        shmid = shmget((key_t)1234, sizeof(struct serverPerform), 0666|IPC_CREAT);
        if(shmid == -1)  
        {  
            perror("shmget failed ");  
            exit(1);;  
        }  
          
        shm = shmat(shmid, 0, 0);  
        if(shm == (void*)-1)  
        {   
            perror("shmat failed ");  
            exit(1);;  
        };  
        printf("the shared memory block was created successfully\n");          
        printf("Memory attached at %X\n", (int)shm);  
       
        sharedM = (struct serverPerform*)shm;  
        bzero(sharedM, sizeof(struct serverPerform));

    }

}



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


void monitor_workload_self()
{
    char tmpStr[256]={0};
    char retMess[256]={0};  


    //it needs to initiate SERVERSNUM servers size memory if it is a master server  
    if (MASTER_FLAG==1)
    {

        bzero(sharedM, SERVERSNUM*sizeof(struct serverPerform));

    }else   //only one server size memory is needed to be initiated for slave server
    {
        bzero(sharedM, sizeof(struct serverPerform));

    };

    //self-updating the workload information, 
    //the data here is for testing on a PC only, because some command lines 
    //are not able to run on a PC. for a real server, please switch to the commented code below(it has been tested on a real server)
    for(;;)
    { 

        if (MASTER_FLAG==1)
        { //master  server updating workload info
            sharedM->phy_cpu_num=2;
            sharedM->log_cpu_num=8;
            sharedM->cpu_num=16;
            sharedM->totalWload=16.000000;
            sharedM->wload1m=0.260000;
            sharedM->wload5m=0.193125;
            sharedM->wload15m=0.125625;
            sharedM->CPUload_us=2.100000;
            sharedM->CPUload_sy=6.100000;
            sharedM->CPUload_ni=0.000000;
            sharedM->CPUload_id=91.800000;
            sharedM->CPUload_wa=0.100000;
            sharedM->MEM_total=64543;
            sharedM->MEM_phy_F=23531; 
            sharedM->MEM_cach_F=18910;
            sharedM->MEM_used=22102;
            sharedM->DISK_us=52;
            sharedM->DISK_fr=48;
            sharedM->slave_index=0;

            //have to be un-commented below if you want to run it on a real server
            

            //have to be un-commented below if you want to run it on a real server
            
            // sharedM->phy_cpu_num=atoi(shellcmd("cat /proc/cpuinfo| grep \"physical id\"| sort| uniq| wc -l", buff, sizeof(buff)));
            // sharedM->log_cpu_num=atoi(shellcmd("cat /proc/cpuinfo | grep \"siblings\"| sort -n | tail -1 |awk '{print $3}'", buff, sizeof(buff)));
            // sharedM->cpu_num=maServer.phy_cpu_num*maServer.log_cpu_num;
            // sharedM->totalWload=maServer.cpu_num*1.00;
            // sharedM->wload1m=atof(shellcmd("echo \"scale=4;tmp1=$(uptime | awk '{print $10}'|cut -f 1 -d \",\");if(length(tmp1)==scale(tmp1)) print 0;print tmp1\" | bc", buff, sizeof(buff)))/sharedM->totalWload;
            // sharedM->wload5m=atof(shellcmd("echo \"scale=4;tmp2=$(uptime | awk '{print $11}'|cut -f 1 -d \",\");if(length(tmp2)==scale(tmp2)) print 0;print tmp2\"| bc", buff, sizeof(buff)))/sharedM->totalWload;
            // sharedM->wload15m=atof(shellcmd("echo \"scale=4;tmp3=$(uptime | awk '{print $NF}'|cut -f 1 -d \",\");if(length(tmp3)==scale(tmp3)) print 0;print tmp3\"| bc", buff, sizeof(buff)))/sharedM->totalWload;
            // sharedM->CPUload_us=atof(shellcmd("echo \"tmp=$(top -b -n 1 | grep Cpu | awk '{print $2}');if(length(tmp)==scale(tmp)) print 0;print tmp\" |bc", buff, sizeof(buff)));
            // sharedM->CPUload_sy=atof(shellcmd("echo \"tmp=$(top -b -n 1 | grep Cpu | awk '{print $4}');if(length(tmp)==scale(tmp)) print 0;print tmp\" |bc", buff, sizeof(buff)));
            // sharedM->CPUload_ni=atof(shellcmd("echo \"tmp=$(top -b -n 1 | grep Cpu | awk '{print $6}');if(length(tmp)==scale(tmp)) print 0;print tmp\" |bc", buff, sizeof(buff)));
            // sharedM->CPUload_id=atof(shellcmd("echo \"tmp=$(top -b -n 1 | grep Cpu | awk '{print $8}');if(length(tmp)==scale(tmp)) print 0;print tmp\" |bc", buff, sizeof(buff)));
            // sharedM->CPUload_wa=atof(shellcmd("echo \"tmp=$(top -b -n 1 | grep Cpu | awk '{print $10}');if(length(tmp)==scale(tmp)) print 0;print tmp\" |bc", buff, sizeof(buff)));
            // sharedM->MEM_total=atol(shellcmd("free -m | grep Mem | awk '{print $2}'", buff, sizeof(buff)));
            // sharedM->MEM_phy_F=atol(shellcmd("free -m | grep Mem | awk '{print $4}'", buff, sizeof(buff)));
            // sharedM->MEM_cach_F=atol(shellcmd("free -m | grep - | awk '{print $NF}'", buff, sizeof(buff)))-sharedM->MEM_phy_F;
            // sharedM->MEM_used=atol(shellcmd("free -m | grep - | awk '{print $3}'", buff, sizeof(buff)));
            // sharedM->DISK_us=atoi(shellcmd("df -h | awk '{print $5}' | grep % | grep -v Use | sort -n | tail -1 | cut -d \"%\" -f1 -", buff, sizeof(buff)));
            // sharedM->DISK_fr=100-sharedM->DISK_us;
            // sharedM->slave_index=SLAVE_INDEX;

            //testing and sending twitter alarm message if it reaches  threashold 
         
            if (sharedM->wload15m > 5.0)
            {
                sprintf(tmpStr,"curl http://tweetapart.com/tweet?api_key=%s'&'status=Server_Workload_15m_reaches_threashold_5.0",Twitter_Api_Key);
                printf("sending twitter mess result: %s",shellcmd(tmpStr, retMess, sizeof(retMess)));

            };

            bzero(tmpStr,sizeof(tmpStr));
            bzero(retMess,sizeof(retMess));
            if (sharedM->CPUload_us > 75)
            {
                sprintf(tmpStr,"curl http://tweetapart.com/tweet?api_key=%s'&'status=Server_CPU_Usage_reaches_threashold_75_Percent",Twitter_Api_Key);
                printf("sending twitter alarm mess result: %s",shellcmd(tmpStr, retMess, sizeof(retMess)));
            };

            bzero(tmpStr,sizeof(tmpStr));
            bzero(retMess,sizeof(retMess));
            if (sharedM->MEM_used > (sharedM->MEM_total*0.75))
            {
                sprintf(tmpStr,"curl http://tweetapart.com/tweet?api_key=%s'&'status=Server_Memory_Usage_reaches_threashold_75_Percent",Twitter_Api_Key);
                printf("sending twitter alarm mess result: %s",shellcmd(tmpStr, retMess, sizeof(retMess)));
            };

            bzero(tmpStr,sizeof(tmpStr));
            bzero(retMess,sizeof(retMess));
            if (sharedM->DISK_us > 75)
            {

                sprintf(tmpStr,"curl http://tweetapart.com/tweet?api_key=%s'&'status=Server_Disk_Usage_reaches_threashold_75_Percent",Twitter_Api_Key);
                printf("sending twitter alarm mess result: %s",shellcmd(tmpStr, retMess, sizeof(retMess)));

            };


        };

        if (MASTER_FLAG==0)
        { //slave updating workload information

            sharedM->phy_cpu_num=2;
            sharedM->log_cpu_num=8;
            sharedM->cpu_num=16;
            sharedM->totalWload=16.000000;
            sharedM->wload1m=1.260000;
            sharedM->wload5m=1.193125;
            sharedM->wload15m=0.785625;
            sharedM->CPUload_us=3.100000;
            sharedM->CPUload_sy=4.100000;
            sharedM->CPUload_ni=1.000000;
            sharedM->CPUload_id=90.800000;
            sharedM->CPUload_wa=0.100000;
            sharedM->MEM_total=64543;
            sharedM->MEM_phy_F=20531; 
            sharedM->MEM_cach_F=21910;
            sharedM->MEM_used=22102;
            sharedM->DISK_us=40;
            sharedM->DISK_fr=60;
            sharedM->slave_index=SLAVE_INDEX;

            //have to be un-commented below if you want to run it on a real server
            
            // sharedM->phy_cpu_num=atoi(shellcmd("cat /proc/cpuinfo| grep \"physical id\"| sort| uniq| wc -l", buff, sizeof(buff)));
            // sharedM->log_cpu_num=atoi(shellcmd("cat /proc/cpuinfo | grep \"siblings\"| sort -n | tail -1 |awk '{print $3}'", buff, sizeof(buff)));
            // sharedM->cpu_num=maServer.phy_cpu_num*maServer.log_cpu_num;
            // sharedM->totalWload=maServer.cpu_num*1.00;
            // sharedM->wload1m=atof(shellcmd("echo \"scale=4;tmp1=$(uptime | awk '{print $10}'|cut -f 1 -d \",\");if(length(tmp1)==scale(tmp1)) print 0;print tmp1\" | bc", buff, sizeof(buff)))/sharedM->totalWload;
            // sharedM->wload5m=atof(shellcmd("echo \"scale=4;tmp2=$(uptime | awk '{print $11}'|cut -f 1 -d \",\");if(length(tmp2)==scale(tmp2)) print 0;print tmp2\"| bc", buff, sizeof(buff)))/sharedM->totalWload;
            // sharedM->wload15m=atof(shellcmd("echo \"scale=4;tmp3=$(uptime | awk '{print $NF}'|cut -f 1 -d \",\");if(length(tmp3)==scale(tmp3)) print 0;print tmp3\"| bc", buff, sizeof(buff)))/sharedM->totalWload;
            // sharedM->CPUload_us=atof(shellcmd("echo \"tmp=$(top -b -n 1 | grep Cpu | awk '{print $2}');if(length(tmp)==scale(tmp)) print 0;print tmp\" |bc", buff, sizeof(buff)));
            // sharedM->CPUload_sy=atof(shellcmd("echo \"tmp=$(top -b -n 1 | grep Cpu | awk '{print $4}');if(length(tmp)==scale(tmp)) print 0;print tmp\" |bc", buff, sizeof(buff)));
            // sharedM->CPUload_ni=atof(shellcmd("echo \"tmp=$(top -b -n 1 | grep Cpu | awk '{print $6}');if(length(tmp)==scale(tmp)) print 0;print tmp\" |bc", buff, sizeof(buff)));
            // sharedM->CPUload_id=atof(shellcmd("echo \"tmp=$(top -b -n 1 | grep Cpu | awk '{print $8}');if(length(tmp)==scale(tmp)) print 0;print tmp\" |bc", buff, sizeof(buff)));
            // sharedM->CPUload_wa=atof(shellcmd("echo \"tmp=$(top -b -n 1 | grep Cpu | awk '{print $10}');if(length(tmp)==scale(tmp)) print 0;print tmp\" |bc", buff, sizeof(buff)));
            // sharedM->MEM_total=atol(shellcmd("free -m | grep Mem | awk '{print $2}'", buff, sizeof(buff)));
            // sharedM->MEM_phy_F=atol(shellcmd("free -m | grep Mem | awk '{print $4}'", buff, sizeof(buff)));
            // sharedM->MEM_cach_F=atol(shellcmd("free -m | grep - | awk '{print $NF}'", buff, sizeof(buff)))-sharedM->MEM_phy_F;
            // sharedM->MEM_used=atol(shellcmd("free -m | grep - | awk '{print $3}'", buff, sizeof(buff)));
            // sharedM->DISK_us=atoi(shellcmd("df -h | awk '{print $5}' | grep % | grep -v Use | sort -n | tail -1 | cut -d \"%\" -f1 -", buff, sizeof(buff)));
            // sharedM->DISK_fr=100-sharedM->DISK_us;
            // sharedM->slave_index=SLAVE_INDEX;

            //testing and sending twitter alarm message if it reaches  threashold

            if (sharedM->wload5m > 5)
            {
                sprintf(tmpStr,"curl http://tweetapart.com/tweet?api_key=%s'&'status=Server_Workload_5m_reaches_threashold_5.0",Twitter_Api_Key);
                printf("sending twitter mess result: %s",shellcmd(tmpStr, retMess, sizeof(retMess)));

            };

            bzero(tmpStr,sizeof(tmpStr));
            bzero(retMess,sizeof(retMess));
            if (sharedM->CPUload_us > 75)
            {
                sprintf(tmpStr,"curl http://tweetapart.com/tweet?api_key=%s'&'status=Server_CPU_Usage_reaches_threashold_75_Percent",Twitter_Api_Key);
                printf("sending twitter alarm mess result: %s",shellcmd(tmpStr, retMess, sizeof(retMess)));
            };

            bzero(tmpStr,sizeof(tmpStr));
            bzero(retMess,sizeof(retMess));
            if (sharedM->MEM_used > (sharedM->MEM_total*0.75))
            {
                sprintf(tmpStr,"curl http://tweetapart.com/tweet?api_key=%s'&'status=Server_Memory_Usage_reaches_threashold_75_Percent",Twitter_Api_Key);
                printf("sending twitter alarm mess result: %s",shellcmd(tmpStr, retMess, sizeof(retMess)));
            };

            bzero(tmpStr,sizeof(tmpStr));
            bzero(retMess,sizeof(retMess));
            if (sharedM->DISK_us > 75)
            {

                sprintf(tmpStr,"curl http://tweetapart.com/tweet?api_key=%s'&'status=Server_Disk_Usage_reaches_threashold_75_Percent",Twitter_Api_Key);
                printf("sending twitter alarm mess result: %s",shellcmd(tmpStr, retMess, sizeof(retMess)));

            };



        };

        sleep(4);


    }



}



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




void process_conn_server(int s)
{
    ssize_t size = 0;
    char sndBuffer[1024];  //sending buffer
    char revBuffer[1024];  //receiving buffer 
    struct serverPerform revSerInfo;

    usleep(100000); 
    for(;;){//loop to handle the request/response

        bzero(sndBuffer, sizeof(sndBuffer));   
        bzero(revBuffer, sizeof(revBuffer));        
        //first time to read the message
        size = read(s, revBuffer, 1024);

        if (size == -1)
        {
            //meeting reading error and continue to read in next loop
            usleep(100000); 
            continue;

        };

        //Client close socket
        if (size == 0)
        {
            printf("client exit\n");
            break; 
        };


        if (strncmp("Android_GET", revBuffer, 4) == 0) 
        {             
            printf("the sizeof struct: %ld\n", sizeof(struct serverPerform));
            printf("got a request from android client: Android_GET \n");
            memcpy(sndBuffer,sharedM,SERVERSNUM*sizeof(struct serverPerform));
            write(s, sndBuffer, SERVERSNUM*sizeof(struct serverPerform));
            printf("response to android client with: %d Servers Workload Info below\n", SERVERSNUM);

            for (int i = 0; i < SERVERSNUM; ++i)
            {
            
                printf("----------------------------------\n");
                printf("------------Server %d------------\n", i);
                printf("----------------------------------\n");
                printf("phy_cpu_num: %d \n", sharedM[i].phy_cpu_num);
                printf("log_cpu_num: %d \n", sharedM[i].log_cpu_num);
                printf("cpu_num: %d \n", sharedM[i].cpu_num);
                printf("totalWload: %f \n", sharedM[i].totalWload);
                printf("wload1m: %f \n", sharedM[i].wload1m);
                printf("wload5m: %f \n", sharedM[i].wload5m);
                printf("wload15m: %f \n", sharedM[i].wload15m);
                printf("CPUload_us: %f \n", sharedM[i].CPUload_us);
                printf("CPUload_sy: %f \n", sharedM[i].CPUload_sy);
                printf("CPUload_ni: %f \n", sharedM[i].CPUload_ni);
                printf("CPUload_id: %f \n", sharedM[i].CPUload_id);
                printf("CPUload_wa: %f \n", sharedM[i].CPUload_wa);
                printf("MEM_total: %ld \n", sharedM[i].MEM_total);
                printf("MEM_phy_F: %ld \n", sharedM[i].MEM_phy_F);
                printf("MEM_cach_F: %ld \n", sharedM[i].MEM_cach_F);
                printf("MEM_used: %ld \n", sharedM[i].MEM_used);
                printf("DISK_us: %d \n", sharedM[i].DISK_us);
                printf("DISK_fr: %d \n", sharedM[i].DISK_fr);
                printf("slave_index: %d \n", sharedM[i].slave_index);                
                printf("----------------------------------\n");
                printf("----------------------------------\n");  

            }

        };

        //accepting the slave updating workload info
        if (strncmp("Updating_Workload_Info", revBuffer, 4) == 0)
        {
            bzero(revBuffer, sizeof(revBuffer));   
            size = read(s, revBuffer, 1024);

            memcpy(&revSerInfo, revBuffer, sizeof(struct serverPerform));
            printf("got a request from a slave server: Updating_Workload_Info \n");
            printf("its slave_index is: %d \n", revSerInfo.slave_index); 

            printf("----------------------------------\n");
            printf("------------Slave Server: %d-------------\n", revSerInfo.slave_index);
            printf("----------------------------------\n");
            printf("phy_cpu_num: %d \n", revSerInfo.phy_cpu_num);
            printf("log_cpu_num: %d \n", revSerInfo.log_cpu_num);
            printf("cpu_num: %d \n", revSerInfo.cpu_num);
            printf("totalWload: %f \n", revSerInfo.totalWload);
            printf("wload1m: %f \n", revSerInfo.wload1m);
            printf("wload5m: %f \n", revSerInfo.wload5m);
            printf("wload15m: %f \n", revSerInfo.wload15m);
            printf("CPUload_us: %f \n", revSerInfo.CPUload_us);
            printf("CPUload_sy: %f \n", revSerInfo.CPUload_sy);
            printf("CPUload_ni: %f \n", revSerInfo.CPUload_ni);
            printf("CPUload_id: %f \n", revSerInfo.CPUload_id);
            printf("CPUload_wa: %f \n", revSerInfo.CPUload_wa);
            printf("MEM_total: %ld \n", revSerInfo.MEM_total);
            printf("MEM_phy_F: %ld \n", revSerInfo.MEM_phy_F);
            printf("MEM_cach_F: %ld \n", revSerInfo.MEM_cach_F);
            printf("MEM_used: %ld \n", revSerInfo.MEM_used);
            printf("DISK_us: %d \n", revSerInfo.DISK_us);
            printf("DISK_fr: %d \n", revSerInfo.DISK_fr);
            printf("slave_index: %d \n", revSerInfo.slave_index);
            printf("----------------------------------\n");
            printf("----------------------------------\n");
            //updating the slave information to the server 
            memcpy(&sharedM[revSerInfo.slave_index], &revSerInfo, sizeof(struct serverPerform));
            printf("updating successfully for slave: %d \n", revSerInfo.slave_index); 

        };


    }  
    //release the socket  
    close(s);

}

/*running on a slave mode, reporting SerPerInfo to master  */
void process_conn_clientSL(int s)
{
    ssize_t size = 0;
    char sndBuffer[1024];  //sending buffer
    char revBuffer[1024];  //receiving buffer
    struct serverPerform *revSerInfo;
    int i;

    sleep(2);

    for(;;){


        bzero(sndBuffer, sizeof(sndBuffer));   
        sprintf(sndBuffer,"Updating_Workload_Info");
        size = write(s, sndBuffer, strlen("Updating_Workload_Info"));
        usleep(100000);    
        bzero(sndBuffer, sizeof(sndBuffer));                  
        memcpy(sndBuffer,sharedM, sizeof(struct serverPerform));
        size = write(s, sndBuffer, sizeof(struct serverPerform));
        printf("requesting master server %s:%d: Updating_Workload_Info \n",SERVERIP,SERVERPORT);

        if (size < 0)
        {

            perror(" writing socket error ");
            break;

        };
        sleep(4);


    }
    //release the socket  
    close(s);

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
            break;

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

    //release the socket  
    close(s);

}

//interrup signal handling function
void sig_proccess(int signo)
{
    printf("Catch a exit signal!\n");
    //release the shared memory
    shmctl(shmid, IPC_RMID, NULL);    
    kill(0,SIGTERM);
    printf("the program will exit\n");
    exit(1);

}


//pipe signal handling function
void sig_pipe(int sign)
{
    //can be different from sig_process according to different requirement
    printf("Catch a SIGPIPE signal!\n");
    //release the shared memory
    shmctl(shmid, IPC_RMID, NULL);    
    kill(0,SIGTERM);
    printf("the program will exit\n");
    exit(1);
    
}
