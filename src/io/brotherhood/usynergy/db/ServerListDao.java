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
			db.execSQL("insert into " + DBHelper.serverlist + "(ipadd,port)values('" + ent.ipadd + "','" + ent.port
					+ "');");
		} finally {
			if (db != null)
				db.close();
		}
	}

	public synchronized void update(ServerEntity obj) {
		try {
			db = this.open();
			db.execSQL("update " + DBHelper.serverlist + " set ipadd = '" + obj.id + "' , port = '" + obj.port
					+ "' where id = " + obj.id);
		} finally {
			if (db != null)
				db.close();
		}
	}

	public synchronized void delete(String id) {
		try {
			db = this.open();
			db.execSQL("delete from " + DBHelper.serverlist + " where id ='" + id + "'");
		} finally {
			if (db != null)
				db.close();
		}
	}
}
