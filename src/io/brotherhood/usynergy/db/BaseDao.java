package io.brotherhood.usynergy.db;

import android.database.sqlite.SQLiteDatabase;

public abstract class BaseDao {
	private DBHelper dbOpenHelper;
	protected SQLiteDatabase db = null;

	public BaseDao() {
		dbOpenHelper = new DBHelper();
	}

	public SQLiteDatabase open() {
		db = dbOpenHelper.getWritableDatabase();
		return db;
	}

	public void close() {
		dbOpenHelper.close();
	}
}
