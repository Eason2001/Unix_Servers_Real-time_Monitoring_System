package com.sysmon.tab.sysmon;

import android.app.Activity;
import android.os.Bundle;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.AdapterView.*;
import android.view.View.OnClickListener;
import android.widget.Adapter.*;
import android.content.Intent;
import android.app.Activity;
import android.os.Bundle;
import android.widget.ListView;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.HashMap;
import android.widget.SimpleAdapter;
import android.view.View;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView;
import android.widget.Adapter;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;
public class MssActivity extends Activity {

    private static List<SerMod> servlist;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_serv);

        ArrayList<String> prefix = getIntent().getStringArrayListExtra("servlist");
        //String prefix = getIntent().getStringExtra("servlist");
        //String[] stmp = prefix.split("/");

        servlist = new ArrayList<SerMod>();
        for (int i = 0; i < prefix.size(); i++)
        {
            SerMod ss = new SerMod(prefix.get(i));
            servlist.add(ss);
        }



        List<String> data = new ArrayList<String>();
        for (int i = 0; i < servlist.size(); i++)
        {
            if (i == 0) data.add("Master");
            else data.add("Branch: " + i);
        }

        ArrayAdapter<String> adapter=new ArrayAdapter<String>(MssActivity.this,android.R.layout.simple_list_item_1, data);
        ListView listView=(ListView) findViewById(R.id.lv_serv);
        listView.setAdapter(adapter);

        listView.setOnItemClickListener(new OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                Intent intent = new Intent(MssActivity.this, DetailActivity.class);
                intent.putExtra("serv", servlist.get(position).ToStringExt());
                startActivity(intent);
            }
        });
    }
}
