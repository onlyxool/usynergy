package io.brotherhood.usynergy.view;

import io.brotherhood.usynergy.R;
import io.brotherhood.usynergy.bean.ServerEntity;

import java.util.List;

import android.app.Activity;
import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.RadioButton;
import android.widget.TextView;

public class ServerListAdapter extends BaseAdapter {
	private static final String TAG = "ServerListAdapter";
	private Activity mContext;
	private ViewHolder holder;
	private List<ServerEntity> list = null;
	private int temp = -1;
	private MyOnCheckedChangeListener listener = null;

	public ServerListAdapter(Activity mContext, List<ServerEntity> mData,int checked) {
		this.mContext = mContext;
		list = mData;
		listener = new MyOnCheckedChangeListener();
		this.temp = checked;
	}

	@Override
	public int getCount() {
		return list.size();
	}

	@Override
	public Object getItem(int position) {
		return list.get(position);
	}

	@Override
	public long getItemId(int position) {
		return list.get(position).id;
	}
	
	public void setList(List<ServerEntity> list){
		this.list = list;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		ServerEntity obj = list.get(position);
		if (convertView == null) {
			final LayoutInflater inflater = (LayoutInflater) mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
			holder = new ViewHolder();
			convertView = inflater.inflate(R.layout.serverlistitem, null);
			holder.ipandport = (TextView) convertView.findViewById(R.id.ipandport);
			holder.select = (RadioButton) convertView.findViewById(R.id.select);
			
			convertView.setTag(holder);
		} else {
			holder = (ViewHolder) convertView.getTag();
		}
		holder.ipandport.setText(obj.ipadd + " : " + obj.port);
		holder.select.setOnCheckedChangeListener(listener);
		holder.select.setId(holder.select.getId()+position);
		if(temp == position){
			holder.select.setChecked(true);
        }
		return convertView;
	}

	private class MyOnCheckedChangeListener implements OnCheckedChangeListener{
		@Override
		public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
			Log.i(TAG, buttonView.getId()+" temp="+temp);
			if (isChecked) {
				if (temp != -1) {
					RadioButton tempButton = (RadioButton) mContext.findViewById(temp);
					if (tempButton != null) {
						tempButton.setChecked(false);
					}
				}
				temp = buttonView.getId();
				
			}else{
				buttonView.setChecked(false);
				
			}
		}
	}

	static class ViewHolder {
		TextView ipandport;
		RadioButton select;
	}
}
