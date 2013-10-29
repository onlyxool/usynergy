package io.brotherhood.usynergy.util;

import io.brotherhood.usynergy.MainActivity;
import io.brotherhood.usynergy.R;
import android.app.Application;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;

public class App extends Application {
	private static App instance;
	private static NotificationManager nm = null;
	// private static final String TAG = "APP";

	public static App getInstance() {
		return instance;
	}

	@Override
	public void onCreate() {
		instance = this;
		nm = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
	}

	
	public void notifiation() {
		Notification n = new Notification(R.drawable.ic_launcher, getString(R.string.app_name),
				System.currentTimeMillis());

		n.flags = Notification.FLAG_ONGOING_EVENT;
		Intent i = new Intent(this, MainActivity.class);
		i.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
		PendingIntent contentIntent = PendingIntent.getActivity(getApplicationContext(), R.string.app_name, i,
				PendingIntent.FLAG_UPDATE_CURRENT);
		n.setLatestEventInfo(this, getString(R.string.app_name), getString(R.string.usynergyisrun), contentIntent);
		nm.notify(R.string.app_name, n);
	}

	public void cancelNotification() {
		nm.cancel(R.string.app_name);
	}
}
