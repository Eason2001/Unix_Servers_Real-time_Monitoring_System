#!/bin/bash
# Written by Chao,Zhang(104656305),March 27,2017
# Usage:Configure server name,server port,server number and twitter account then show configuration.
#       1.configure local server
#       2.configure cluster server
#       3.configure twitter account
#       4.show local server configuration
#       5.show cluster server configuration
#       6.show twitter configuration

max=1             # the current max number of cluster servers

menu()                # user interface function
{
clear
scriptname="Config.sh"    
version=2.1.0
date=`date +%F.%T`

cat <<MENULIST
================================================================================

ScriptName:$scriptname        Version:$version         Date&Time:$date

================================================================================
This shell script can automatically complete the following configuration:

        1.configure local server
        2.configure master and cluster server
		3.configure twitter account
        4.check local server configuration
        5.check master and cluster server configuration
        6.check twitter configuration
		
================================================================================

MENULIST

echo -n "Please input your choice [1,2,3,4,5,6,b(back),q(quit)]:"    # first input choice
read choice
}

function RewriteFile()     # rewrite function
  {   
      sed -i "/^\[$1\]/,/^\[/ {/^\$1\]/b;/^\[/b;s/^$2*=.*/$2=$3/g;}" $4      # find tag $1 target key string $2 change the value after = with $3 in file $4
  }  

function ReadFile()       # read function
  { 
      awk -F '=' '/\['$1'\]/{a=1}a==1&&$1~/'$2'/{print $2;exit}' $3    # find tag $1 target key string $2 read the value after = in file $3
  }
  
  
  
function localServer()    # choice 1  
{
     echo
     echo "Start configuring local server."
     echo "Guide:" 
     echo "Please enter local server name."         
     read lsname
     echo "Please enter local server IP address."
     read lIP
     echo "Please enter local server port."	 
	 read lport
	 echo "Please enter if local server is master server(1=YES/0=NO)."  # if local server is not master enter the number of it in the cluster
	 read ljudger
	 if [ "$ljudger" -eq 0 ] 
	 then
	       echo "Please enter the number of secondary server local server has(integer from 2 to 10)." 
           read lsnumber
		   while( [ "$lsnumber" -le 2 ] || [ "$lsnumber" -ge 10 ] )
		   do
		       echo -n "Please re-enter a interger from 2 to 10."
		       read lsnumber
		   done
		   RewriteFile "Local_Server.config" "Secondary_Index" $lsnumber "./Server.config"   #do rewritefile
     fi 
	 RewriteFile "Local_Server.config" "Local_Server_Name" $lsname "./Server.config"
	 RewriteFile "Local_Server.config" "Local_Server_IP" $lIP "./Server.config"
	 RewriteFile "Local_Server.config" "Local_Server_Port" $lport "./Server.config"
	 RewriteFile "Local_Server.config" "Master_Flag" $ljudger "./Server.config"
	 
	 echo "Finish configuration."
	 echo
}

function clusterServer()   #choice 2
{



     echo
     echo "Start configuring cluster server."
     echo "Guide:" 
     echo "Please enter cluster server name."
	 read csname
	 echo -n "Please enter the number of computers in the cluster(Input an integer from 1 to 10):"
         read cnumber
	 RewriteFile "Cluster_Server.config" "Cluster_Name" $csname "./Server.config"
	 RewriteFile "Cluster_Server.config" "Number_Of_Servers" $cnumber "./Server.config"
	 if [ "$cnumber" -ge 1 ]&&[ "$cnumber" -le 10 ] 
	 then
	    for ((i=1;i<=$cnumber;i++))
		do  
		    if [ $i -eq 1 ]
			then 
			
			      echo "Please enter master server name."
				  read  csname[$i]
				  echo "Please enter master server IP address."
				  read  cIP[$i]
				  echo "Please enter master server port."
				  read  cport[$i]
				  RewriteFile "Cluster_Server.config" "Master_Server" ${csname[$i]} "./Server.config"    
	              RewriteFile "Cluster_Server.config" "Master_Server_IP" ${cIP[$i]} "./Server.config"
	              RewriteFile "Cluster_Server.config" "Master_Server_Port" ${cport[$i]} "./Server.config"
		    else
			      grep -q "Secondary_Server_($i)_Name" ./Server.config  
			       if [ $? -ne 0 ] && [ $i -gt $max ]  #if i is bigger than current max and secondary server i doesn't exist
				   then
			      cat <<EOF >> Server.config    #append new secondary server information
Secondary_Server_($i)_Name=defaultserver
Secondary_Server_($i)_IP=0.0.0.0
Secondary_Server_($i)_Port=0
EOF
			      max=$[$max+1] # max=max+1
			     
		           fi
			       
			      echo "Please enter secondary server $i name."
				  read  csname[$i]
				  echo "Please enter secondary server $i IP address."
				  read  cIP[$i]
				  echo "Please enter secondary server $i port."
				  read  cport[$i]
				  RewriteFile "Cluster_Server.config" "Secondary_Server_($i)_Name" ${csname[$i]} "./Server.config"
	              RewriteFile "Cluster_Server.config" "Secondary_Server_($i)_IP" ${cIP[$i]} "./Server.config"
	              RewriteFile "Cluster_Server.config" "Secondary_Server_($i)_Port" ${cport[$i]} "./Server.config"
		    fi
	    done
	 echo "Finish configuration."
	 else
	    echo "Please enter an integer from 1 to 10."
		echo "Go back to menu."
	 fi
}

function checkLocalServer() #choice 4
{
     readlsname=$(ReadFile "Local_Server.config" "Local_Server_Name" "./Server.config") # do readfile and send parameters
	 readlIP=$(ReadFile "Local_Server.config" "Local_Server_IP" "./Server.config")
	 readlport=$(ReadFile "Local_Server.config" "Local_Server_Port" "./Server.config")
     echo  
	 echo "Start listing local server information."
     echo "Local server name      :$readlsname"
     echo "Local server IP address:$readlIP"
     echo "Local server port      :$readlport"	 
	 echo "Finish list."
	 echo
}

function checkClusterServer() #choice 5
{
     echo  
	 echo "Start listing cluster server information."
	 readclustername=$(ReadFile "Cluster_Server.config" "Cluster_Name" "./Server.config")
	 readclusternumber=$(ReadFile "Cluster_Server.config" "Number_Of_Servers" "./Server.config")
	 echo "Cluster   server  name        :$readclustername"
     echo "Number of servers in cluster  :$readclusternumber"
	 echo
	 let scounter=`grep "Secondary_Server_" ./Server.config|wc -l`/3
     for ((k=1;k<=scounter+1;k++))  #use array to receive values of cluster servers which are read from file
	 do
	        if [ $k -eq 1 ] 
			then 
			      readcsname[1]=$(ReadFile "Cluster_Server.config" "Master_Server_Name" "./Server.config")
	              readcIP[1]=$(ReadFile "Cluster_Server.config" "Master_Server_IP" "./Server.config")
	              readcport[1]=$(ReadFile "Cluster_Server.config" "Master_Server_Port" "./Server.config")
			      echo "Master    server  name        :${readcsname[1]}"	
				  echo "Master    server  IP address  :${readcIP[1]}"
				  echo "Master    server  port        :${readcport[1]}"
				  echo
		    elif [ $k -eq 2 ]
			then
			      readcsname[2]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(2)_Name" "./Server.config")
	              readcIP[2]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(2)_IP" "./Server.config")
	              readcport[2]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(2)_Port" "./Server.config")
				  echo "Secondary server $k name       :${readcsname[2]}"	
				  echo "Secondary server $k IP address :${readcIP[2]}"
				  echo "Secondary server $k port       :${readcport[2]}"
				  echo
            elif [ $k -eq 3 ]
			then
			      readcsname[3]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(3)_Name" "./Server.config")
	              readcIP[3]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(3)_IP" "./Server.config")
	              readcport[3]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(3)_Port" "./Server.config")
				  echo "Secondary server $k name       :${readcsname[3]}"	
				  echo "Secondary server $k IP address :${readcIP[3]}"
				  echo "Secondary server $k port       :${readcport[3]}"
				  echo
			elif [ $k -eq 4 ]
			then
			      readcsname[4]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(4)_Name" "./Server.config")
	              readcIP[4]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(4)_IP" "./Server.config")
	              readcport[4]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(4)_Port" "./Server.config")
				  echo "Secondary server $k name       :${readcsname[4]}"	
				  echo "Secondary server $k IP address :${readcIP[4]}"
				  echo "Secondary server $k port       :${readcport[4]}"
				  echo
			elif [ $k -eq 5 ]
			then
			      readcsname[5]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(5)_Name" "./Server.config")
	              readcIP[5]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(5)_IP" "./Server.config")
	              readcport[5]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(5)_Port" "./Server.config")
				  echo "Secondary server $k name       :${readcsname[5]}"	
				  echo "Secondary server $k IP address :${readcIP[5]}"
				  echo "Secondary server $k port       :${readcport[5]}"
				  echo
			elif [ $k -eq 6 ]
			then
			      readcsname[6]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(6)_Name" "./Server.config")
	              readcIP[6]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(6)_IP" "./Server.config")
	              readcport[6]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(6)_Port" "./Server.config")
				  echo "Secondary server $k name       :${readcsname[6]}"	
				  echo "Secondary server $k IP address :${readcIP[6]}"
				  echo "Secondary server $k port       :${readcport[6]}"
				  echo
			elif [ $k -eq 7 ]
			then
			      readcsname[7]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(7)_Name" "./Server.config")
	              readcIP[7]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(7)_IP" "./Server.config")
	              readcport[7]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(7)_Port" "./Server.config")
				  echo "Secondary server $k name       :${readcsname[7]}"	
				  echo "Secondary server $k IP address :${readcIP[7]}"
				  echo "Secondary server $k port       :${readcport[7]}"
				  echo
			elif [ $k -eq 8 ]
			then
			      readcsname[8]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(8)_Name" "./Server.config")
	              readcIP[8]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(8)_IP" "./Server.config")
	              readcport[8]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(8)_Port" "./Server.config")
				  echo "Secondary server $k name       :${readcsname[8]}"	
				  echo "Secondary server $k IP address :${readcIP[8]}"
				  echo "Secondary server $k port       :${readcport[8]}"
				  echo
			elif [ $k -eq 9 ]
			then
			      readcsname[9]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(9)_Name" "./Server.config")
	              readcIP[9]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(9)_IP" "./Server.config")
	              readcport[9]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(9)_Port" "./Server.config")
				  echo "Secondary server $k name       :${readcsname[9]}"	
				  echo "Secondary server $k IP address :${readcIP[9]}"
				  echo "Secondary server $k port       :${readcport[9]}"
				  echo
			elif [ $k -eq 10 ]
			then
			      readcsname[10]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(10)_Name" "./Server.config")
	              readcIP[10]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(10)_IP" "./Server.config")
	              readcport[10]=$(ReadFile "Cluster_Server.config" "Secondary_Server_(10)_Port" "./Server.config")
				  echo "Secondary server $k name       :${readcsname[10]}"	
				  echo "Secondary server $k IP address :${readcIP[10]}"
				  echo "Secondary server $k port       :${readcport[10]}"
				  echo
		    fi
	 done
	 echo "Finish list."
	 echo
}

function twitter() #choice 3
{
     echo  
	 echo "Start configure twitter account."
	 echo "Guide:" 
     echo "Please enter twitter name."
	 read twitterName
     echo "Please enter twitter api key."
	 read twitterApiKey
	 echo "Finish configuration."
	 echo
	 RewriteFile "Twitter.config" "Twitter_Name" $twitterName "./Server.config"
     RewriteFile "Twitter.config" "Twitter_Api_Key" $twitterApiKey "./Server.config"
}

function checkTwitter() #choice 6
{
     readtname=$(ReadFile "Twitter.config" "Twitter_Name" "./Server.config")
	 readtapi=$(ReadFile "Twitter.config" "Twitter_Api_Key" "./Server.config")
     echo  
	 echo "Start listing twitter information."
     echo "Twitter name      :$readtname"
     echo "Twitter api key   :$readtapi"
	 echo "Finish list."
	 echo
}

if [ ! -f "Server.config" ] #check if the file exist
then 
     cat <<EOF >> Server.config  #create and append default values
[Local_Server.config]
Local_Server_Name=Alpha_Server
Local_Server_IP=127.0.0.1
Local_Server_Port=18888
Master_Flag=-1
Secondary_Index=1

[Twitter.config]
Twitter_Name=Chao_Zhang
Twitter_Api_Key=0

[Cluster_Server.config]
Cluster_Name=Center_Cluster
Number_Of_Servers=1
Master_Server_Name=defaultserver
Master_Server_IP=127.0.0.1
Master_Server_Port=0
EOF
fi


for ((j=1;;j++))   # main function
do menu
case "$choice" in
        "1")
                localServer
                ;;
        "2")
                clusterServer
                ;;
        "3")
                twitter
                ;;
        "4")
                checkLocalServer
                ;;
        "5")
                checkClusterServer
                ;;
		"6")
                checkTwitter
                ;;
        "b")
                unset choice  #back
                ;;
        "q")
                exit 0   #quit
                ;;
esac
if [ ! "$choice" = "" ]
then
        echo "Press any key to return!"
        read 
fi
done


