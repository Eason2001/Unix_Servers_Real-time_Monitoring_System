package com.sysmon.tab.sysmon;

import java.util.List;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

/**
 * Created by Tab on 2017-03-21.
 */

public class ServAdapter extends BaseAdapter{

    private Context context;

    private List<SerMod> servlist;

    public ServAdapter(Context context, List<SerMod> apkks ) {
        super();
        this.context =  context;
        this.servlist = apkks;
    }

    @Override
    public int getCount() {
        return servlist.size();
    }

    @Override
    public Object getItem(int position) {
        return position;
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    static private class ViewHolder{
        public ImageView servicon;
        public TextView servname;
    }

    @Override
    public View getView(final int position, View convertView, ViewGroup parent) {

        ViewHolder holder;
        if (convertView == null) {
            holder = new ViewHolder();
            LayoutInflater inflater = LayoutInflater.from(context);
            convertView = inflater.inflate(R.layout.adapter_serv, null);
            holder.servicon = (ImageView)convertView.findViewById(R.id.iv_serv_icon);
            holder.servname = (TextView)convertView.findViewById(R.id.tv_serv_title);
            convertView.setTag(holder);
        }else {
            holder=(ViewHolder) convertView.getTag();
        }



        if (position == 0) {
            //holder.servicon.setImageBitmap(R.drawable.class.);
        } else {

        }




        holder.servname.setText("Branch:" + position);

        return convertView;
    }


}
