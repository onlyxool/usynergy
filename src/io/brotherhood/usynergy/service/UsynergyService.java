package io.brotherhood.usynergy.service;

import io.brotherhood.usynergy.MainActivity;
import android.app.Service;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.IBinder;
import android.preference.PreferenceManager;

public class UsynergyService extends Service {
	private SharedPreferences sharePre = null;
	
    @Override
    public void onCreate() {
    	sharePre = PreferenceManager.getDefaultSharedPreferences(this);
    	String screenName = sharePre.getString("screenname","android");
    	int width = sharePre.getInt(MainActivity.KEY_WIDTH, 0);
    	int height = sharePre.getInt(MainActivity.KEY_HEIGHT, 0);
        init(screenName,height,width);
        start("192.168.0.102", 24800);
    }

    @Override
    public void onDestroy() {
    	super.onDestroy();
    	shutdown();
    	exit();
    }

	@Override
	public IBinder onBind(Intent arg0) {
		return null;
	}

	public native int init(String screenName,int hight,int width);
	public native int start(String ip,int port);
	public native int shutdown();
	public native int setClientName(String clientName,int height,int width);
	public native int exit();
	public native String getClipBoardText();

	static{
		System.loadLibrary("usynercore");
	}
}
