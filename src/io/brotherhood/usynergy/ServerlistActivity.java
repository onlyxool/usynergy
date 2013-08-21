package io.brotherhood.usynergy;

import io.brotherhood.usynergy.bean.ServerEntity;
import io.brotherhood.usynergy.db.ServerListDao;
import io.brotherhood.usynergy.view.ServerListAdapter;

import java.util.List;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnCreateContextMenuListener;
import android.widget.EditText;
import android.widget.ListView;

import com.actionbarsherlock.app.SherlockActivity;
import com.actionbarsherlock.view.Menu;
import com.actionbarsherlock.view.MenuItem;

public class ServerlistActivity extends SherlockActivity implements OnCreateContextMenuListener {
	private static final String tag = "ServerlistActivity";
	private final String MENU_ADD = "add";
	private ListView serverlistView = null;
	private List<ServerEntity> serverList = null;
	private ServerListDao dao = null;
	private ServerListAdapter adapter = null;
	private SharedPreferences sharePre = null;
//	private final String CHECKED = "checked";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
    	setTheme(R.style.Theme_Sherlock);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_serverlist);
		sharePre = PreferenceManager.getDefaultSharedPreferences(this);

        dao = new ServerListDao();
        serverList = dao.getList();

        adapter = new ServerListAdapter(this,serverList,sharePre);

        serverlistView = (ListView)findViewById(R.id.serverlist);
        serverlistView.setAdapter(adapter);
        serverlistView.setOnCreateContextMenuListener(this);
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
		Log.i(tag, item.getItemId()+"#"+item.getTitle());
		switch (item.getItemId()) {
		case android.R.id.home:
			this.finish();
			break;
		case 0:
			showAddAndEditDialog(null);
			break;
		case R.id.edit:
//			toEdit(position);
			break;
		case R.id.del:
//			toDel(position);
			break;
		default:
			break;
		}
		return super.onOptionsItemSelected(item);
	}
	
	private void showAddAndEditDialog(final ServerEntity ent) {
		if(isFinishing())
			return;
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		LayoutInflater factory = LayoutInflater.from(this);
		final View textEntryView = factory.inflate(R.layout.add_alert_dialog, null);
		final EditText ipadd = (EditText) textEntryView.findViewById(R.id.ipadd);
		final EditText port = (EditText) textEntryView.findViewById(R.id.port);
//		ipadd.requestFocusFromTouch();
		if(ent!=null){
			ipadd.setText(ent.ipadd);
			port.setText(ent.port);
		}
		builder.setIconAttribute(R.drawable.ic_launcher).setTitle(R.string.addnewserverconfig).setView(textEntryView)
			.setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
				public void onClick(DialogInterface dialog, int whichButton) {
					if(ent==null){
						ServerEntity obj = new ServerEntity();
						obj.ipadd = ipadd.getText().toString();
						obj.port = port.getText().toString();
						dao.save(obj);
						reloadList();
					}else{
						ent.ipadd = ipadd.getText().toString();
						ent.port = port.getText().toString();
						dao.update(ent);
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

	private void reloadList(){
		serverList = dao.getList();
		adapter.setList(serverList);
		adapter.notifyDataSetChanged();
	}

}
