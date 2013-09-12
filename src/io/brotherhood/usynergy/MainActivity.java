package io.brotherhood.usynergy;

import io.brotherhood.usynergy.bean.Screen;
import io.brotherhood.usynergy.bean.ServerEntity;
import io.brotherhood.usynergy.db.ServerListDao;
import io.brotherhood.usynergy.service.UsynergyService;
import io.brotherhood.usynergy.util.PhoneUtils;
import io.brotherhood.usynergy.util.RootCmd;
import android.app.AlertDialog;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.os.Bundle;
import android.preference.EditTextPreference;
import android.preference.Preference;
import android.preference.PreferenceManager;
import android.preference.PreferenceScreen;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;

import com.actionbarsherlock.app.SherlockPreferenceActivity;
import com.actionbarsherlock.view.Menu;
import com.actionbarsherlock.view.MenuItem;

public class MainActivity extends SherlockPreferenceActivity implements OnSharedPreferenceChangeListener {
	private final String tag = "MainActivity";
	private SharedPreferences sharePre = null;
	public static final String KEY_WIDTH = "width";
	public static final String KEY_HEIGHT = "height";
	private int width = 0;
	private int height = 0;
	public static final String defualtScreenName = "android";
	private ServerListDao dao = null;
	private NotificationManager nm = null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		getSupportActionBar().setDisplayHomeAsUpEnabled(true);
		addPreferencesFromResource(R.xml.main_preference);
		getPreferenceScreen().getSharedPreferences().registerOnSharedPreferenceChangeListener(this);

		sharePre = PreferenceManager.getDefaultSharedPreferences(this);
		width = sharePre.getInt(KEY_WIDTH, 0);
		height = sharePre.getInt(KEY_HEIGHT, 0);
		if (width == 0 || height == 0) {
			Screen s = PhoneUtils.getResolution(this);
			Editor editor = sharePre.edit();
			editor.putInt(KEY_WIDTH, s.width);
			editor.putInt(KEY_HEIGHT, s.height);
			editor.commit();
		}

		String screenNameKey = getString(R.string.screenname);
		EditTextPreference screenname = (EditTextPreference) findPreference(screenNameKey);
		screenname.setSummary(sharePre.getString(screenNameKey, defualtScreenName));

		dao = new ServerListDao();

		boolean haveRoot = RootCmd.rootCmd("chmod 666 /dev/uinput");
		Log.i(tag, "root=" + haveRoot);
		if (!haveRoot) {

		}
		nm = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
	}

	@Override
	protected void onResume() {
		super.onResume();
		ServerEntity obj = dao.getSelectObj();
		if (obj != null) {
			String serverListKey = getString(R.string.serverlist);
			PreferenceScreen serverlist = (PreferenceScreen) findPreference(serverListKey);
			serverlist.setSummary(obj.getFullAddress());
		}
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case android.R.id.home:
			this.finish();
			break;
		default:
			break;
		}
		return super.onOptionsItemSelected(item);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		return super.onCreateOptionsMenu(menu);
	}

	@Override
	public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
		String key = preference.getKey();
		if (key == null)
			return false;
		Log.i(tag, key);
		if (key.equals(getString(R.string.start))) {
			Boolean startOption = sharePre.getBoolean(getString(R.string.start), false);
			Log.e(tag, startOption + "");
			if (startOption){
				ComponentName service = startService(new Intent(this, UsynergyService.class));
				if (service == null) {
					Log.e(tag, "Can't start service " + UsynergyService.class.getName());
				}
				notifiation();
			}else{
				cancelNotification();
			}
		} else if (key.equals(getString(R.string.serverlist))) {
			Intent intent = new Intent(this, ServerlistActivity.class);
			intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
			startActivity(intent);
		} else if (key.equals(getString(R.string.about))) {
			showAboutDialog();
		}
		return super.onPreferenceTreeClick(preferenceScreen, preference);
	}

	private void showAboutDialog() {
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		LayoutInflater factory = LayoutInflater.from(this);
		final View dialogView = factory.inflate(R.layout.about_alert_dialog, null);
		builder.setTitle(getString(R.string.about));
		builder.setView(dialogView).setCancelable(true);
		builder.show();
	}

	@Override
	public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
		Preference pref = findPreference(key);
		if (pref instanceof EditTextPreference) {
			EditTextPreference etp = (EditTextPreference) pref;
			pref.setSummary(etp.getText());
		}
	}

	public void notifiation() {
		Notification n = new Notification(R.drawable.ic_launcher, this.getString(R.string.app_name),
				System.currentTimeMillis());

		n.flags = Notification.FLAG_ONGOING_EVENT;
		Intent i = new Intent(this, MainActivity.class);
		i.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
		PendingIntent contentIntent = PendingIntent.getActivity(this, R.string.app_name, i,
				PendingIntent.FLAG_UPDATE_CURRENT);
		n.setLatestEventInfo(this, getString(R.string.app_name), getString(R.string.usynergyisrun), contentIntent);
		nm.notify(R.string.app_name, n);
	}

	public void cancelNotification(){
		nm.cancel(R.string.app_name);
	}
}
