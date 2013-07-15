package io.brotherhood.usynergy.util;

import android.app.Application;

public class App extends Application {
	private static App instance;
//	private static final String TAG = "APP";

	public static App getInstance() {
		return instance;
	}

	@Override
	public void onCreate() {
		instance = this;
	}


}
