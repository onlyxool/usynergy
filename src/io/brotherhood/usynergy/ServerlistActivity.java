package io.brotherhood.usynergy;

import io.brotherhood.usynergy.bean.ServerEntity;
import io.brotherhood.usynergy.db.ServerListDao;
import io.brotherhood.usynergy.view.ServerListAdapter;

import java.util.List;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.util.Log;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnCreateContextMenuListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.EditText;
import android.widget.ListView;

import com.actionbarsherlock.app.SherlockActivity;
import com.actionbarsherlock.view.Menu;
import com.actionbarsherlock.view.MenuItem;

public class ServerlistActivity extends SherlockActivity implements OnCreateContextMenuListener, OnItemClickListener {
	private static final String tag = "ServerlistActivity";
	private final String MENU_ADD = "add";
	private ListView serverlistView = null;
	private List<ServerEntity> serverList = null;
	private ServerListDao dao = null;
	private ServerListAdapter adapter = null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		setTheme(R.style.Theme_Sherlock);
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_serverlist);
		getSupportActionBar().setDisplayHomeAsUpEnabled(true);

		dao = new ServerListDao();
		serverList = dao.getList();

		adapter = new ServerListAdapter(this, serverList);

		serverlistView = (ListView) findViewById(R.id.serverlist);
		serverlistView.setAdapter(adapter);
		serverlistView.setOnCreateContextMenuListener(this);
		serverlistView.setOnItemClickListener(this);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		menu.add(MENU_ADD).setIcon(R.drawable.ic_compose_inverse).setShowAsAction(MenuItem.SHOW_AS_ACTION_IF_ROOM);
		return super.onCreateOptionsMenu(menu);
	}

	@Override
	public void onCreateContextMenu(ContextMenu menu, View v, ContextMenuInfo menuInfo) {
		getMenuInflater().inflate(R.menu.main, menu);
		super.onCreateContextMenu(menu, v, menuInfo);
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		Log.i(tag, item.getItemId() + "#" + item.getTitle());
		switch (item.getItemId()) {
		case android.R.id.home:
			this.finish();
			break;
		case 0:
			showAddAndEditDialog(null);
			break;
		default:
			break;
		}
		return super.onOptionsItemSelected(item);
	}

	public boolean onContextItemSelected(android.view.MenuItem item) {
		AdapterView.AdapterContextMenuInfo itemInfo = (AdapterView.AdapterContextMenuInfo) item.getMenuInfo();
		ServerEntity obj = serverList.get(itemInfo.position);
		switch (item.getItemId()) {
		case R.id.edit:
			showAddAndEditDialog(obj);
			break;
		case R.id.del:
			delItem(obj);
			break;
		default:
			break;
		}
		return super.onContextItemSelected(item);
	}

	private void showAddAndEditDialog(final ServerEntity obj) {
		if (isFinishing())
			return;
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		LayoutInflater factory = LayoutInflater.from(this);
		final View textEntryView = factory.inflate(R.layout.add_alert_dialog, null);
		final EditText ipadd = (EditText) textEntryView.findViewById(R.id.ipadd);
		final EditText port = (EditText) textEntryView.findViewById(R.id.port);
		if (obj != null) {
			ipadd.setText(obj.ipadd);
			port.setText(obj.port);
			builder.setTitle(R.string.editserverconfig);
		} else {
			port.setText(R.string.defualtport);
			builder.setTitle(R.string.addnewserverconfig);
		}
		builder.setIconAttribute(R.drawable.ic_launcher).setView(textEntryView)
				.setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int whichButton) {
						if (obj == null) {
							ServerEntity newObj = new ServerEntity();
							newObj.ipadd = ipadd.getText().toString();
							newObj.port = port.getText().toString();
							dao.save(newObj);
							reloadList();
						} else {
							obj.ipadd = ipadd.getText().toString();
							obj.port = port.getText().toString();
							dao.update(obj);
							reloadList();
						}
						dialog.dismiss();
					}
				}).setNegativeButton(R.string.cancel, new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int whichButton) {
						dialog.dismiss();
					}
				}).show();
	}

	private void delItem(ServerEntity obj){
		dao.delete(obj.id);
		reloadList();
	}

	private void reloadList() {
		serverList = dao.getList();
		adapter.setList(serverList);
		adapter.notifyDataSetChanged();
	}

	@Override
	public void onItemClick(AdapterView<?> arg0, View arg1, int arg2, long arg3) {
		ServerEntity obj = serverList.get(arg2);
		dao.selectByObj(obj);
		reloadList();
	}
}
