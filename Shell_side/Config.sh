#!/bin/bash
# Written by Chao,Zhang(104656305),March 5,2017
# Usage:configure server


counter=1

menu()
{
clear
scriptname="Config.sh"    ##it is better to name it: Config.sh
version=1.1.0
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

echo -n "Please input your choice [1,2,3,4,5,6,b(back),q(quit)]:"
read choice1
}

function RewriteFile()  
  {   
      sed -i "/^\[$1\]/,/^\[/ {/^\$1\]/b;/^\[/b;s/^$2*=.*/$2=$3/g;}" $4
  }  

function ReadFile()
  { 
      awk -F '=' '/\['$1'\]/{a=1}a==1&&$1~/'$2'/{print $2;exit}' $3
  }
  
  
  
function localServer()
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
	 ##调用RewriteFile函数时有问题，写不了，第四个参数是一个保存文本文件，这写法是不是不对？
	 # 保存文件建议命名为 Server.config, 区分当前的脚本文件 Config.sh，带sh一般是可执行文件
	 # 调试了一下，提示sed的正则表达式写法有问题，可以百度一下sed 正则表达式用法，
	 # 把上面函数的格式读懂，就知道哪里调用错了、传的参数是否正确等等。这个函数OK了，基本就OK了。
	 RewriteFile "Local_Server.config" "Local_Server_Name" $lsname "./Server.config"
	 RewriteFile "Local_Server.config" "Local_Server_IP" $lIP "./Server.config"
	 RewriteFile "Local_Server.config" "Local_Server_Port" $lport "./Server.config"
	 echo "Finish configuration."
	 echo
}

function clusterServer()
{
    # 保存文件建议命名为 Server.config, 区分当前的脚本文件 Config.sh，带sh一般是可执行文件

# 集群配置建议格式:
# [Cluster_Server.config]
# Cluster_Name=Center_Cluster
# Number_Of_Servers=3
    
# Master_Server=defaultserver
# Master_Server_IP=127.0.0.1
# Master_Server_Port=0



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
			       if [ $? -ne 0 ] && [ $i -gt $counter ]
				   then
			      cat <<EOF >> Server.config
Secondary_Server_($i)_Name=defaultserver
Secondary_Server_($i)_IP=0.0.0.0
Secondary_Server_($i)_Port=0
EOF
			      counter=$[$counter+1]
			     
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

function checkLocalServer()
{
     readlsname=$(ReadFile "Local_Server.config" "Local_Server_Name" "./Server.config")
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

function checkClusterServer()
{
     echo  
	 echo "Start listing cluster server information."
	 readclustername=$(ReadFile "Cluster_Server.config" "Cluster_Name" "./Server.config")
	 readclusternumber=$(ReadFile "Cluster_Server.config" "Number_Of_Servers" "./Server.config")
	 echo "Cluster   server  name        :$readclustername"
     echo "Number of servers in cluster  :$readclusternumber"
	 echo
     for ((k=1;k<=i;k++))
	 do
	        if [ $k -eq 1 ]
			then 
			#readcsname[$k]=$(ReadFile "Cluster_Server.config" "Master_Server_Name" "./Server.config")
	        #readcIP[$k]=$(ReadFile "Cluster_Server.config" "Master_Server_IP" "./Server.config")
	        #readcport[$k]=$(ReadFile "Cluster_Server.config" "Master_Server_Port" "./Server.config")
			      echo "Master    server  name        :${csname[$k]}"	
				  echo "Master    server  IP address  :${cIP[$k]}"
				  echo "Master    server  port        :${cport[$k]}"
				  echo
		    else 
			      #readcsname[$k]=$(ReadFile "Twitter.config" "Twitter_Name" "./Server.config")
	              #readcIP[$k]=$(ReadFile "Cluster_Server.config" "Secondary_Server_($k)_IP" "./Server.config")
	              #readcport[$k]=$(ReadFile "Cluster_Server.config" "Secondary_Server_($k)_Port" "./Server.config")
			      echo "Secondary server $k name       :${csname[$k]}"	
				  echo "Secondary server $k IP address :${cIP[$k]}"
				  echo "Secondary server $k port       :${cport[$k]}"
				  echo
		    fi
	 done
	 echo "Finish list."
	 echo
}

function twitter()
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

function checkTwitter()
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

if [ ! -f "Server.config" ]
then 
     cat <<EOF >> Server.config
[Local_Server.config]
Local_Server_Name=Alpha_Server
Local_Server_IP=127.0.0.1
Local_Server_Port=18888

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

# 本地服务器配置建议格式:
# [Local_Server.config]
# Local_Server_Name=Alpha_Server
# Local_Server_IP=127.0.0.1
# Local_Server_Port=18888


for ((j=1;;j++))
do menu
case "$choice1" in
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
                unset choice1
                ;;
        "q")
                exit 0
                ;;
esac
if [ ! "$choice1" = "" ]
then
        echo "Press any key to return!"
        read 
fi
done


