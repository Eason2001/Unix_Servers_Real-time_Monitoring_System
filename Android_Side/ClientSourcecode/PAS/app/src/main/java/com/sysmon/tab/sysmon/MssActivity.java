package com.sysmon.tab.sysmon;

import android.app.Activity;
import android.os.Bundle;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.content.Intent;
import android.view.View;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView;

import java.util.ArrayList;
import java.util.List;

public class MssActivity extends Activity {

    private static List<SerMod> servlist;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_serv);

        ArrayList<String> prefix = getIntent().getStringArrayListExtra("servlist");

        servlist = new ArrayList<SerMod>();
        for (int i = 0; i < prefix.size(); i++)
        {
            SerMod ss = new SerMod(prefix.get(i));
            servlist.add(ss);
        }



        List<String> data = new ArrayList<String>();
        for (int i = 0; i < servlist.size(); i++)
        {
            if (servlist.get(i).slave_index == 0) data.add("Master");
            else data.add("Slave: " + servlist.get(i).slave_index);
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
