package io.brotherhood.usynergy.db;

import io.brotherhood.usynergy.util.App;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

public class DBHelper extends SQLiteOpenHelper {
	private static final String name = "usynergy.db";
	public static final String serverlist = "serverlist";

	public DBHelper() {
		super(App.getInstance(), name, null, 1);
	}

	@Override
	public void onCreate(SQLiteDatabase db) {
		db.execSQL("create table " + serverlist + "(id integer primary key autoincrement,ipadd varchar(20),"
				+ "port varchar(5),checked integer);");
	}

	@Override
	public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
		db.execSQL("DROP TABLE IF EXISTS " + serverlist);
		onCreate(db);
	}
}
