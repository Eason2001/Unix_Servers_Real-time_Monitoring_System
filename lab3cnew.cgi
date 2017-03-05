#!/bin/bash


# ZHICHENG PAN, 104639463
# Lab 3c - MONITORING SYSTEM PERFORMENCE

# load the library functions
source ./lab3lib.cgi


if [  "x$username" != "x"  ]; then
	
		if [[ $username == "admin" && $userpasswd == "12345" ]]; then


			#For calculating system work load performance: using bar chart
			declare -i phy_cpu_num=1
			declare -i log_cpu_num=1
			declare -i cpu_num=1

			phy_cpu_num=$(cat /proc/cpuinfo| grep "physical id"| sort| uniq| wc -l)
			#each physical CPU has many logical CPUs
			log_cpu_num=$(cat /proc/cpuinfo | grep "siblings"| sort -n | tail -1 |awk '{print $3}')
			#total logical CPUs:
			cpu_num=$phy_cpu_num*$log_cpu_num
			# echo "cpu_num: "$cpu_num

			#defining a float point variable to carry the total system performance
			totalWload=$(echo "$cpu_num*1.0" | bc)
			# echo "totalWload: "$totalWload

			wload1m=$(echo "scale=4;tmp1=$(uptime | awk '{print $10}'|cut -f 1 -d ",")/$totalWload;if(length(tmp1)==scale(tmp1)) print 0;print tmp1" | bc)
			# echo "wload1m: "$wload1m
			wload5m=$(echo "scale=4;tmp2=$(uptime | awk '{print $11}'|cut -f 1 -d ",")/$totalWload;if(length(tmp2)==scale(tmp2)) print 0;print tmp2"| bc)
			# echo "wload5m: "$wload5m
			wload15m=$(echo "scale=4;tmp3=$(uptime | awk '{print $NF}'|cut -f 1 -d ",")/$totalWload;if(length(tmp3)==scale(tmp3)) print 0;print tmp3"| bc)
			# echo "wload15m: "$wload15m

			#For calculating system CPU performance: using pie chart
			CPUload_us=$(echo "tmp=$(top -b -n 1 | grep Cpu | awk '{print $2}');if(length(tmp)==scale(tmp)) print 0;print tmp" |bc)
			# echo "CPUload_us: "$CPUload_us

			CPUload_sy=$(echo "tmp=$(top -b -n 1 | grep Cpu | awk '{print $4}');if(length(tmp)==scale(tmp)) print 0;print tmp" |bc)
			# echo "CPUload_sy: "$CPUload_sy

			CPUload_ni=$(echo "tmp=$(top -b -n 1 | grep Cpu | awk '{print $6}');if(length(tmp)==scale(tmp)) print 0;print tmp" |bc)
			# echo "CPUload_ni: "$CPUload_ni

			CPUload_id=$(echo "tmp=$(top -b -n 1 | grep Cpu | awk '{print $8}');if(length(tmp)==scale(tmp)) print 0;print tmp" |bc)
			# echo "CPUload_id: "$CPUload_id

			CPUload_wa=$(echo "tmp=$(top -b -n 1 | grep Cpu | awk '{print $10}');if(length(tmp)==scale(tmp)) print 0;print tmp" |bc)
			# echo "CPUload_wa: "$CPUload_wa


			#For calculating system memory usuage: using pie chart
			let MEM_total=$(free -m | grep Mem | awk '{print $2}')
			#echo "MEM_total: "$MEM_total
			let MEM_phy_F=$(free -m | grep Mem | awk '{print $4}')
			#echo "MEM_phy_F: "$MEM_phy_F
			let MEM_cach_F=$(free -m | grep - | awk '{print $NF}')-$MEM_phy_F
			#echo "MEM_cach_F: "$MEM_cach_F
			let MEM_used=$(free -m | grep - | awk '{print $3}')
			#echo "MEM_used: "$MEM_used

			#For calculating system disk usuage: using pie chart
			let DISK_us=$(df -h | awk '{print $5}' | grep % | grep -v Use | sort -n | tail -1 | cut -d "%" -f1 -)
			# echo "DISK_us: "$DISK_us
			let DISK_fr=100-$DISK_us
			# echo "DISK_fr: "$DISK_fr

			#Generate the HTML file:


cat <<EOFA
<html>
<head>
<meta charset="UTF-8">
<script src="./src/Chart.bundle.js"></script>
<title>Monitoring System Performence</title>
</head>
<body>
<h1 align="center">System Realtime Performence</h1>
<table width="750" height="518" border="0" align="center" cellpadding="8" cellspacing="8">
<tbody>
<tr valign="bottom">
<td align="center">System average workload for <span style="color:red">1, 5, 15 </span> minutes</td>
<td align="center">CPU usage in different part:<span style="color:red"> % </span></td>
</tr>
<tr>
<td width="184" align="center" valign="middle">
<div style="overflow: auto" >  
<div style="width:300px; height:400px;">
<canvas id="myChart1" ></canvas>
<script type="text/javascript">
var ctx1 = document.getElementById("myChart1");
var myChart1 = new Chart(ctx1, {
type: 'bar',
data: {
labels: ["1 minute", "5 minutes", "15 minutes"],
datasets: [{
label: '#Greater than 5 is not healthy!',
data: [${wload1m}, ${wload5m}, ${wload15m}],
backgroundColor: [
'rgba(255, 99, 132, 0.2)',
'rgba(54, 162, 235, 0.2)',
'rgba(255, 206, 86, 0.2)',
],
borderColor: [
'rgba(255,99,132,1)',
'rgba(54, 162, 235, 1)',
'rgba(255, 206, 86, 1)' ],
borderWidth: 1
}]
},
options: {
scales: {
yAxes: [{
ticks: {
beginAtZero:true,
max: 6,
min: 0
<!--stepSize: 2.5-->
}
}]
}
}
});
</script>
</div>
</div>			
</td>
<td width="168" align="center" valign="middle">
<!-- using css(style) to resize divs that contain charts-->
<div style="overflow: auto" >  
<div style="width:280px; height:280px;">
<canvas id="myChart3" ></canvas>
<script type="text/javascript">
var ctx3 = document.getElementById("myChart3");
var myChart3 = new Chart(ctx3, {
type: 'pie',
data: {
labels: [
"Application",
"System",
"Network",
"Idle",
"Wait"
],
datasets: [
{
label: '#CPU usage in different part',
data: [${CPUload_us}, ${CPUload_sy}, ${CPUload_ni}, ${CPUload_id}, ${CPUload_wa}],
backgroundColor: [
"#FF6384",
"#36A2EB",
"#FFCE56",
"#FFAB84",
"#3610EB"
],
hoverBackgroundColor: [
"#FF6384",
"#36A2EB",
"#FFCE56",
"#FFAB84",
"#3610EB"
]
}]
},
});
</script>
</div></div>
</td>
</tr>
<tr valign="bottom">
<td align="center">Memory usage: <span style="color:red">Uint M </span></td>
<td align="center">Storage Usage: <span style="color:red">%  </span></td>
</tr>
<tr>
<td align="center" valign="middle">
<!-- using css(style) to resize divs that contain charts-->
<div style="overflow: auto" >  
<div style="width:280px; height:280px;">
<canvas id="myChart2" ></canvas>
<script type="text/javascript">
var ctx2 = document.getElementById("myChart2");
var myChart2 = new Chart(ctx2, {
type: 'pie',
data: {
labels: [
"Physical Free",
"Cache Free",
"Used Memory"
],
datasets: [
{
label: '#Memory usage',
data: [${MEM_phy_F}, ${MEM_cach_F}, ${MEM_used}],
backgroundColor: [
"#FF6384",
"#36A2EB",
"#FFCE56",
],
hoverBackgroundColor: [
"#FF6384",
"#36A2EB",
"#FFCE56"
]
}]
},
});
</script>
</div></div>
</td>
<td align="center" valign="middle">
<!-- using css(style) to resize divs that contain charts-->
<div style="overflow: auto" >  
<div style="width:280px; height:280px;">
<canvas id="myChart4" ></canvas>
<script type="text/javascript">
var ctx4 = document.getElementById("myChart4");
var myChart4 = new Chart(ctx4, {
type: 'pie',
data: {
labels: [
"Disk used",
"Disk free"
],
datasets: [
{
label: '#Storage Usage',
data: [${DISK_us}, ${DISK_fr}],
backgroundColor: [
"#FF6384",
"#36A2EB"
],
hoverBackgroundColor: [
"#FF6384",
"#36A2EB"
]
}]
},
});
</script>
</div></div>
</td>
</tr>
<tr>
<td align="center" valign="middle">&nbsp;</td>
<td align="center" valign="middle">&nbsp;</td>
</tr>
</tbody>
</table>
</body>
</html>
EOFA
		else

cat <<EOFC
<html>
<body>
<span style="color:red">!!!Please enter correct username and password and try again.  </span><br/><br/>
<form action="?version=${version:=1}" method="POST" enctype="application/x-www-form-urlencoded">
Administrator: <input type="text" name="username" value="$username"><br/><br/>
Password: <input type="password" name="userpasswd" value="$userpasswd"><br/><br/>
<input type="submit">
</form>
</body>
</html>
EOFC


		fi


#showing the login page:
else

cat <<EOFB
<html>
<body>
Please use administrator account to login the server:<br/><br/>
<form action="?version=${version:=1}" method="POST" enctype="application/x-www-form-urlencoded">
Administrator: <input type="text" name="username" value="$username"><br/><br/>
Password: <input type="password" name="userpasswd" value="$userpasswd"><br/><br/>
<input type="submit">
</form>
</body>
</html>
EOFB

fi