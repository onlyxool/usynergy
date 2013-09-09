package io.brotherhood.usynergy.service;

import io.brotherhood.usynergy.MainActivity;
import io.brotherhood.usynergy.R;
import io.brotherhood.usynergy.bean.ServerEntity;
import io.brotherhood.usynergy.db.ServerListDao;
import android.app.Service;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.IBinder;
import android.preference.PreferenceManager;
import android.util.Log;
import android.widget.Toast;

public class UsynergyService extends Service {
	private final String tag = "UsynergyService";
	private SharedPreferences sharePre = null;
	private ServerListDao dao = null;
	private RunSynergyThread runSynergyThread = null;
	private String screenName = null;
	private int width = 0;
	private int height = 0;
	private ServerEntity obj = null;

	@Override
	public void onCreate() {
		sharePre = PreferenceManager.getDefaultSharedPreferences(this);
		String screenNameKey = getString(R.string.screenname);
		screenName = sharePre.getString(screenNameKey, MainActivity.defualtScreenName);
		width = sharePre.getInt(MainActivity.KEY_WIDTH, 0);
		height = sharePre.getInt(MainActivity.KEY_HEIGHT, 0);
		dao = new ServerListDao();
		obj = dao.getSelectObj();

		if (obj == null) {
			Toast.makeText(this, R.string.noserverconfig, Toast.LENGTH_SHORT).show();
			return;
		}

		runSynergyThread = new RunSynergyThread();
		runSynergyThread.start();
	}

	class RunSynergyThread extends Thread {
		public void run() {
			init(screenName, height, width);
			int result = UsynergyService.this.start(obj.ipadd, Integer.parseInt(obj.port));
			Log.e(tag, "result=" + result);
		}
	};

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

	public native int init(String screenName, int hight, int width);

	public native int start(String ip, int port);

	public native int shutdown();

	public native int setClientName(String clientName, int height, int width);

	public native int exit();

	public native String getClipBoardText();

	static {
		System.loadLibrary("usynercore");
	}
}
