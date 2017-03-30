package com.sysmon.tab.sysmon;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.content.Intent;

import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.List;
import java.util.ArrayList;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.charset.Charset;

public class MainActivity extends Activity {

    private int MdfySS = 136;
    private int MAX_BUFFER = 2048;
    private Button btn_connect;
    private Button btn_gtlist;
    private EditText et_ip;
    private TextView tv_status;

    private boolean isConnecting = false;

    private Thread mThread = null;
    private Socket mSocket = null;

    private InputStream mInps = null;
    private PrintWriter mPrintWriter = null;
    private String recvMessage = "";

    private List<SerMod> servlist = new ArrayList<SerMod>();

    private Handler tHandler = new Handler()
    {
        public void handleMessage(Message msg)
        {
            super.handleMessage(msg);
            if(msg.what == 0)
            {
                tv_status.setText("Received " + (recvMessage.length()) + " bytes\n" + "-->Msg: " + recvMessage + "\n" + tv_status.getText().toString());
            }
            else if(msg.what == -1)
            {
                tv_status.setText("Received Server Data: " + servlist.size() + " servers are online\n" + tv_status.getText().toString());
                //tv_status.setText(servlist.get(0).ToStringExt() + "---" + servlist.get(1).ToStringExt());
                btn_gtlist.setEnabled(true);
            }
            else if(msg.what == 1)
            {
                tv_status.setText("IP address cannot be empty!!!\n");
            }
            else if(msg.what == 2)
            {
                tv_status.setText("Fake IP Address!!!\n");
            }
            else if(msg.what == 3)
            {
                tv_status.setText("Connected Succed!!!\n");
            }
            else if(msg.what == 4)
            {
                tv_status.setText("IP Error!!!\n" + recvMessage + "\n");
            }
            else
            {
                tv_status.setText("Received Error：" + recvMessage + "\n" + tv_status.getText().toString());
            }
        }
    };

    private Runnable mRunnable = new Runnable()
    {
        public void run()
        {
            String msgText = et_ip.getText().toString();
            if(msgText.length() <= 0)
            {
                Message msg = new Message();
                msg.what = 1;
                tHandler.sendMessage(msg);
                return;
            }

            int start = msgText.indexOf(":");
            if( (start == -1) ||(start + 1 >= msgText.length()) )
            {
                Message msg = new Message();
                msg.what = 2;
                tHandler.sendMessage(msg);
                return;
            }

            String sIP = msgText.substring(0, start);
            String sPort = msgText.substring(start + 1);
            int port = Integer.parseInt(sPort);
            try
            {
                mSocket = new Socket(sIP, port);

                mInps = mSocket.getInputStream();

                //mBufferedReader = new BufferedReader(new InputStreamReader(mInps, "UTF-8"));
                mPrintWriter = new PrintWriter(mSocket.getOutputStream(), true);

                mPrintWriter.print("Android——GET");
                mPrintWriter.flush();

                Message msg = new Message();
                msg.what = 3;
                tHandler.sendMessage(msg);
            }
            catch (Exception e)
            {
                recvMessage = e.getMessage();
                Message msg = new Message();
                msg.what = 4;
                tHandler.sendMessage(msg);
                return;
            }

            byte[] buffer = new byte[MAX_BUFFER];
            int count = 0;
            while (isConnecting)
            {
                try
                {
                    count = mInps.read(buffer);
                    if(count > 0) {
                        if (count != 1 && count % MdfySS == 0) {
                            servlist = new ArrayList<SerMod>();
                            int ctmp = count / MdfySS;
                            for (int i = 0; i < ctmp; i++) {
                                byte[] ele = new byte[MdfySS];
                                System.arraycopy(buffer, i * MdfySS, ele, 0, MdfySS);
                                SerMod tmp = new SerMod(ele);
                                servlist.add(tmp);
                            }
                            Message msg = new Message();
                            msg.what = -1;
                            tHandler.sendMessage(msg);
                        }
                        else {
                            recvMessage = char2string(getChars(buffer), count);
                            Message msg = new Message();
                            msg.what = 0;
                            tHandler.sendMessage(msg);
                        }
                    }
                }
                catch (Exception e)
                {
                    recvMessage = e.getMessage();
                    Message msg = new Message();
                    msg.what = -2;
                    tHandler.sendMessage(msg);
                }
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        et_ip = (EditText)findViewById(R.id.et_ip);
        tv_status = (TextView)findViewById(R.id.tv_status);
        btn_connect = (Button)findViewById(R.id.btn_connect);
        btn_gtlist = (Button)findViewById(R.id.btn_list);

        btn_connect.setEnabled(true);
        btn_gtlist.setEnabled(false);

        //et_ip.setText("192.168.1.58:18888");

        btn_connect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v)
            {
                if (isConnecting)
                {
                    isConnecting = false;
                    try {
                        if(mSocket != null)
                        {
                            mSocket.close();
                            mSocket = null;
                            mPrintWriter.close();
                            mPrintWriter = null;
                        }
                    } catch (IOException e) {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                    }
                    mThread.interrupt();
                    btn_connect.setText("Connect");
                    et_ip.setEnabled(true);
                }
                else
                {
                    isConnecting = true;
                    btn_connect.setText("Disconnect");
                    et_ip.setEnabled(false);
                    mThread = new Thread(mRunnable);
                    mThread.start();
                }
            }
        });

        btn_gtlist.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v)
            {
                ArrayList<String> msg = new ArrayList<String>();
                for (int i = 0; i < servlist.size(); i++) {
                    msg.add(servlist.get(i).ToStringExt());
                }
                Intent intent = new Intent();
                intent.putStringArrayListExtra("servlist", msg);
                intent.setClass(MainActivity.this, MssActivity.class);
                startActivity(intent);
            }
        });
    }

    public static double bytes2Double(byte[] arr) {
        long value = 0;
        for (int i = 0; i < 8; i++) {
            value |= ((long) (arr[i] & 0xff)) << (8 * i);
        }
        return Double.longBitsToDouble(value);
    }

    public static byte[] double2Bytes(double d) {
        long value = Double.doubleToRawLongBits(d);
        byte[] byteRet = new byte[8];
        for (int i = 0; i < 8; i++) {
            byteRet[i] = (byte) ((value >> 8 * i) & 0xff);
        }
        return byteRet;
    }

    private byte[] getBytes (char[] chars) {
        Charset cs = Charset.forName ("UTF-8");
        CharBuffer cb = CharBuffer.allocate (chars.length);
        cb.put (chars);
        cb.flip ();
        ByteBuffer bb = cs.encode (cb);

        return bb.array();

    }
    private static String byte2hex(byte [] buffer){
        String h = "";

        for(int i = 0; i < buffer.length; i++){
            String temp = Integer.toHexString(buffer[i] & 0xFF);
            if(temp.length() == 1){
                temp = "0" + temp;
            }
            h = h + " "+ temp;
        }

        return h;

    }
    private char[] getChars (byte[] bytes) {
        Charset cs = Charset.forName ("UTF-8");
        ByteBuffer bb = ByteBuffer.allocate (bytes.length);
        bb.put (bytes);
        bb.flip ();
        CharBuffer cb = cs.decode (bb);

        return cb.array();
    }

    private String char2string(char[] buff, int count) {
        char[] temp = new char[count];
        for(int i = 0; i < count; i++)
        {
            temp[i] = buff[i];
        }
        return new String(temp);
    }

    public void onDestroy() {
        super.onDestroy();
        if (isConnecting)
        {
            isConnecting = false;
            try {
                if(mSocket != null)
                {
                    mSocket.close();
                    mSocket = null;
                    mPrintWriter.close();
                    mPrintWriter = null;
                }
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            mThread.interrupt();
        }
    }

}
