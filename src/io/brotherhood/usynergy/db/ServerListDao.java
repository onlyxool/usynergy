package io.brotherhood.usynergy.db;

import io.brotherhood.usynergy.bean.ServerEntity;

import java.util.ArrayList;
import java.util.List;

import android.database.Cursor;

public class ServerListDao extends BaseDao {

	public ServerListDao() {
		super();
	}

	public synchronized List<ServerEntity> getList() {
		List<ServerEntity> list = new ArrayList<ServerEntity>();
		ServerEntity entity = null;
		Cursor cursor = null;
		try {
			db = this.open();
			cursor = db.rawQuery("select * from " + DBHelper.serverlist + " order by id desc", null);
			while (cursor.moveToNext()) {
				entity = new ServerEntity();
				entity.id = cursor.getInt(0);
				entity.ipadd = cursor.getString(1);
				entity.port = cursor.getString(2);
				entity.checked = cursor.getInt(3);
				list.add(entity);
			}
			return list;
		} finally {
			if (null != cursor)
				cursor.close();
			if (db != null)
				db.close();
		}
	}

	public synchronized void save(ServerEntity ent) {
		try {
			db = this.open();
			db.execSQL("insert into " + DBHelper.serverlist + "(ipadd,port,checked)values('" + ent.ipadd + "','"
					+ ent.port + "'," + ent.checked + ");");
		} finally {
			if (db != null)
				db.close();
		}
	}

	public synchronized void update(ServerEntity obj) {
		try {
			db = this.open();
			db.execSQL("update " + DBHelper.serverlist + " set ipadd = '" + obj.ipadd + "' , port = '" + obj.port
					+ "',checked=" + obj.checked + " where id = " + obj.id);
		} finally {
			if (db != null)
				db.close();
		}
	}

	public synchronized void selectByObj(ServerEntity obj) {
		try {
			db = this.open();
			db.beginTransaction();
			db.execSQL("update " + DBHelper.serverlist + " set checked=0 where checked=1");
			db.execSQL("update " + DBHelper.serverlist + " set checked=1 where id=" + obj.id);
			db.setTransactionSuccessful();
			db.endTransaction();
		} finally {
			if (db != null)
				db.close();
		}
	}

	public synchronized ServerEntity getSelectObj(){
		ServerEntity obj = null;
		Cursor cursor = null;
		try {
			db = this.open();
			cursor = db.rawQuery("select * from " + DBHelper.serverlist + " where checked = 1", null);
			while (cursor.moveToNext()) {
				obj = new ServerEntity();
				obj.id = cursor.getInt(0);
				obj.ipadd = cursor.getString(1);
				obj.port = cursor.getString(2);
				obj.checked = cursor.getInt(3);
			}
			return obj;
		} finally {
			if (null != cursor)
				cursor.close();
			if (db != null)
				db.close();
		}
	}

	public synchronized void delete(int id) {
		try {
			db = this.open();
			db.execSQL("delete from " + DBHelper.serverlist + " where id ='" + id + "'");
		} finally {
			if (db != null)
				db.close();
		}
	}
}
