package com.sysmon.tab.sysmon;

import android.app.Activity;
import android.os.Bundle;
import android.webkit.WebView;
import android.webkit.WebSettings;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.webkit.WebChromeClient;

import java.util.ArrayList;
import java.util.List;

public class DetailActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_detail);

        String prefix = getIntent().getStringExtra("serv");
        SerMod ss = new SerMod(prefix);

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
        wv.setWebChromeClient(new WebChromeClient());


        //wv.addJavascriptInterface(DetailActivity.this, "callByJs");


        //context.getClass().getClassLoader().getResourceAsStream("assets/"+资源名);
        wv.loadUrl("file:///android_asset/Chat.html");
        //wv.LoadUrl("javascript:alert('hello js')");

        //wv.addJavascriptInterface(new AndroidForJs(this), "JavaScriptInterface");
    }


}
