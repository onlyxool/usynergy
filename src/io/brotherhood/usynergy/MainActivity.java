package io.brotherhood.usynergy;

import io.brotherhood.usynergy.bean.Screen;
import io.brotherhood.usynergy.service.UsynergyService;
import io.brotherhood.usynergy.util.PhoneUtils;
import io.brotherhood.usynergy.util.RootCmd;
import android.content.ComponentName;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.preference.Preference;
import android.preference.PreferenceManager;
import android.preference.PreferenceScreen;
import android.util.Log;

import com.actionbarsherlock.app.SherlockPreferenceActivity;
import com.actionbarsherlock.view.Menu;
import com.actionbarsherlock.view.MenuItem;

public class MainActivity extends SherlockPreferenceActivity {

	private final String tag = "MainActivity";
	private SharedPreferences sharePre = null;
	public static final String KEY_WIDTH = "width";
	public static final String KEY_HEIGHT = "height";
	private int width = 0;
	private int height = 0;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		getSupportActionBar().setDisplayHomeAsUpEnabled(true);
		addPreferencesFromResource(R.xml.main_preference);
		sharePre = PreferenceManager.getDefaultSharedPreferences(this);
		width = sharePre.getInt(KEY_WIDTH, 0);
		height = sharePre.getInt(KEY_HEIGHT, 0);
		if(width==0||height==0){
			Screen s = PhoneUtils.getResolution(this);
			Editor editor = sharePre.edit();
			editor.putInt(KEY_WIDTH, s.width);
			editor.putInt(KEY_HEIGHT, s.height);
			editor.commit();
		}
		boolean haveRoot = RootCmd.rootCmd("ls");
//		Log.i(tag, sharePre.getString("Screen Name", "xxx"));
//		Log.i(tag, sharePre.getBoolean("start", false)+"");
		Log.i(tag, haveRoot+"");
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

	public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
		String key = preference.getKey();
		if(key==null)
			return false;
		Log.i(tag , key);
		if(key.equals(getString(R.string.start))){
			ComponentName service = startService(new Intent(this,UsynergyService.class));
	        if (service == null) {
	            Log.e(tag, "Can't start service " + UsynergyService.class.getName());
	        }
		}else if(key.equals(getString(R.string.serverlist))){
			Intent intent = new Intent(this, ServerlistActivity.class);
			intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
			startActivity(intent);
		}
		return super.onPreferenceTreeClick(preferenceScreen, preference);
	}


}
