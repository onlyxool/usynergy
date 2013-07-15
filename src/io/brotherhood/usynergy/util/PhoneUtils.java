package io.brotherhood.usynergy.util;

import io.brotherhood.usynergy.bean.Screen;
import android.app.Activity;
import android.view.Display;

public class PhoneUtils {

	/**
	 * 获取分辨率
	 */
	public static Screen getResolution(Activity context) {
		Display display = context.getWindowManager().getDefaultDisplay();
		Screen s = new Screen();
		s.width = display.getWidth();
		s.height = display.getHeight();
		return s;
	}

}
