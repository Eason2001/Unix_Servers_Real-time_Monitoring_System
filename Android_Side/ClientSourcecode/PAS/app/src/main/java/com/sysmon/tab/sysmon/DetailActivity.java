package com.sysmon.tab.sysmon;

import android.app.Activity;
import android.os.Bundle;
import android.webkit.WebView;
import android.webkit.WebSettings;
import android.webkit.WebChromeClient;

public class DetailActivity extends Activity {

    final String mimeType = "text/html";
    final String encoding = "utf-8";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_detail);

        String prefix = getIntent().getStringExtra("serv");
        SerMod dt = new SerMod(prefix);

        WebView wv = (WebView)findViewById(R.id.wv_detail);
        WebSettings settings = wv.getSettings();
        settings.setDefaultTextEncodingName("utf-8");
        settings.setJavaScriptEnabled(true);
        settings.setLoadWithOverviewMode(true);
        settings.setSupportZoom(true);
        settings.setDomStorageEnabled(true);
        settings.setCacheMode(WebSettings.LOAD_NO_CACHE);
        settings.setAllowFileAccess(true);
        settings.setUseWideViewPort(true);
        settings.setSupportMultipleWindows(true);
        settings.setBlockNetworkImage(false);
        //wv.setWebChromeClient(new WebChromeClient());

        //wv.loadUrl("file:///android_asset/Chat.html");

        String title = "";
        if (dt.slave_index == 0) title = "Master";
        else title = "Slave " + dt.slave_index;

        String s0 = "<html><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no\"><script src=\"file:///android_asset/Chart.bundle.js\"></script><title>Monitoring System Performence</title></head><body><h1 align=\"center\">System Realtime Performence</h1>" + title + "<table border=\"0\" align=\"center\" cellpadding=\"8\" cellspacing=\"8\"><tbody><tr valign=\"bottom\"><td align=\"center\">System average workload for <span style=\"color:red\">1, 5, 15 </span> minutes</td></tr><tr><td align=\"center\" valign=\"middle\"><div><div><iframe class=\"chartjs-hidden-iframe\" tabindex=\"-1\" style=\"display: block; overflow: hidden; border: 0px; margin: 0px; top: 0px; left: 0px; bottom: 0px; right: 0px; height: 100%; width: 100%; position: absolute; pointer-events: none; z-index: -1;\"></iframe><canvas id=\"myChart1\" width=\"352\" height=\"176\" style=\"display: block; width: 352px; height: 176px;\"></canvas><script type=\"text/javascript\">";
        String s1 = "var wload1m = " + dt.wload1m + ";";
        String s2 = "var wload5m = " + dt.wload5m + ";";
        String s3 = "var wload15m = " + dt.wload15m + ";";

        String s4 = "var ctx1 = document.getElementById(\"myChart1\");var myChart1 = new Chart(ctx1, {type: 'bar',data: {labels: [\"1 minute\", \"5 minutes\", \"15 minutes\"],datasets: [{label: '#Greater than 5 is not healthy!',data: [wload1m, wload5m, wload15m],backgroundColor: ['rgba(255, 99, 132, 0.2)','rgba(54, 162, 235, 0.2)','rgba(255, 206, 86, 0.2)',],borderColor: ['rgba(255,99,132,1)','rgba(54, 162, 235, 1)','rgba(255, 206, 86, 1)' ],borderWidth: 1}]},options: {scales: {yAxes: [{ticks: {beginAtZero:true,max: 6,min: 0}}]}}});</script></div></div></td></tr><tr><td align=\"center\">CPU usage in different part:<span style=\"color:red\"> % </span></td></tr><tr><td align=\"center\" valign=\"middle\"><div><div><iframe class=\"chartjs-hidden-iframe\" tabindex=\"-1\" style=\"display: block; overflow: hidden; border: 0px; margin: 0px; top: 0px; left: 0px; bottom: 0px; right: 0px; height: 100%; width: 100%; position: absolute; pointer-events: none; z-index: -1;\"></iframe><canvas id=\"myChart3\" width=\"352\" height=\"352\" style=\"display: block; width: 352px; height: 352px;\"></canvas><script type=\"text/javascript\">";

        String s5 = "var CPUload_us = " + dt.CPUload_us + ";";
        String s6 = "var CPUload_sy = " + dt.CPUload_sy + ";";
        String s7 = "var CPUload_ni = " + dt.CPUload_ni + ";";
        String s8 = "var CPUload_id = " + dt.CPUload_id + ";";
        String s9 = "var CPUload_wa = " + dt.CPUload_wa + ";";


        String s10 = "var ctx3 = document.getElementById(\"myChart3\");var myChart3 = new Chart(ctx3, {type: 'pie',data: {labels: [\"Application\",\"System\",\"Network\",\"Idle\",\"Wait\"],datasets: [{label: '#CPU usage in different part',data: [CPUload_us, CPUload_sy, CPUload_ni, CPUload_id, CPUload_wa],backgroundColor: [\"#FF6384\",\"#36A2EB\",\"#FFCE56\",\"#FFAB84\",\"#3610EB\"],hoverBackgroundColor: [\"#FF6384\",\"#36A2EB\",\"#FFCE56\",\"#FFAB84\",\"#3610EB\"]}]},});</script></div></div></td></tr><tr valign=\"bottom\"><td align=\"center\">Memory usage: <span style=\"color:red\">Uint M </span></td></tr><tr><td align=\"center\" valign=\"middle\"><div><div><iframe class=\"chartjs-hidden-iframe\" tabindex=\"-1\" style=\"display: block; overflow: hidden; border: 0px; margin: 0px; top: 0px; left: 0px; bottom: 0px; right: 0px; height: 100%; width: 100%; position: absolute; pointer-events: none; z-index: -1;\"></iframe><canvas id=\"myChart2\" style=\"display: block; width: 352px; height: 352px;\" width=\"352\" height=\"352\"></canvas><script type=\"text/javascript\">";

        String s11 = "var MEM_phy_F = " + dt.MEM_phy_F + ";";
        String s12 = "var MEM_cach_F = " + dt.MEM_cach_F + ";";
        String s13 = "var MEM_used = " + dt.MEM_used + ";";

        String s14 = "var ctx2 = document.getElementById(\"myChart2\");var myChart2 = new Chart(ctx2, {type: 'pie',data: {labels: [\"Physical Free\",\"Cache Free\",\"Used Memory\"],datasets: [{label: '#Memory usage',data: [MEM_phy_F, MEM_cach_F, MEM_used],backgroundColor: [\"#FF6384\",\"#36A2EB\",\"#FFCE56\",],hoverBackgroundColor: [\"#FF6384\",\"#36A2EB\",\"#FFCE56\"]}]},});</script></div></div></td></tr><tr><td align=\"center\">Storage Usage: <span style=\"color:red\">%  </span></td></tr><tr><td align=\"center\" valign=\"middle\"><div><div><iframe class=\"chartjs-hidden-iframe\" tabindex=\"-1\" style=\"display: block; overflow: hidden; border: 0px; margin: 0px; top: 0px; left: 0px; bottom: 0px; right: 0px; height: 100%; width: 100%; position: absolute; pointer-events: none; z-index: -1;\"></iframe><canvas id=\"myChart4\" style=\"display: block; width: 352px; height: 352px;\" width=\"352\" height=\"352\"></canvas><script type=\"text/javascript\">";

        String s15 = "var DISK_us = " + dt.DISK_us + ";";
        String s16 = "var DISK_fr = " + dt.DISK_fr + ";";

        String s17 = "var ctx4 = document.getElementById(\"myChart4\");var myChart4 = new Chart(ctx4, {type: 'pie',data: {labels: [\"Disk used\",\"Disk free\"],datasets: [{label: '#Storage Usage',data: [DISK_us, DISK_fr],backgroundColor: [\"#FF6384\",\"#36A2EB\"],hoverBackgroundColor: [\"#FF6384\",\"#36A2EB\"]}]},});</script></div></div></td></tr><tr><td align=\"center\" valign=\"middle\">&nbsp;</td></tr></tbody></table></body></html>";



        String webdata = s0 + s1 + s2 + s3 + s4 + s5 + s6 + s7 + s8 + s9 + s10 + s11 + s12 + s13 + s14 + s15 + s16 + s17;
        wv.loadDataWithBaseURL("Realtime Server Performance", webdata, mimeType, encoding, "");

        //wv.addJavascriptInterface(new AndroidForJs(this), "JavaScriptInterface");
    }


}
