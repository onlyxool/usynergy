package io.brotherhood.usynergy.util;

import io.brotherhood.usynergy.bean.Screen;
import android.app.Activity;
import android.view.Display;

public class PhoneUtils {

	/**
	 * get screen size
	 */
	public static Screen getResolution(Activity context) {
		Display display = context.getWindowManager().getDefaultDisplay();
		Screen s = new Screen();
		s.width = display.getWidth();
		s.height = display.getHeight();
		return s;
	}

}
