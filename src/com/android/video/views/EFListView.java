package com.android.video.views;

import java.lang.reflect.Constructor;

import android.content.Context;
import android.database.DataSetObserver;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListAdapter;
import android.widget.ListView;

public class EFListView extends ListView implements ListAdapter {

	public static interface EFListDataSource {
		public Class<? extends View> getListCellClass() ;
	    public int  getListDataCount() ;   
	    public void fillListViewAtIndex(int index, ViewGroup parent, View cellView, int width, int height) ;
	}
	
	protected int				listRowHeight  = 100 ;
	protected EFListDataSource	listDataSource = null ;
	
	public EFListView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		initListView() ;
	}

	public EFListView(Context context, AttributeSet attrs) {
		super(context, attrs);
		initListView() ;
	}

	public EFListView(Context context) {
		super(context);
		initListView() ;
	}

	protected void initListView() {
		this.setAdapter(this) ;
	}
	
	public void registerDataSetObserver(DataSetObserver observer) {

	}

	public void unregisterDataSetObserver(DataSetObserver observer) {

	}
	
	public int getCount() {
		if(listDataSource == null)
			return 0 ;
		return listDataSource.getListDataCount() ;
	}

	public Object getItem(int position) {
		return null;
	}

	public long getItemId(int position) {
		return 0;
	}

	public boolean hasStableIds() {
		return false;
	}

	public View getView(int position, View convertView, ViewGroup parent) {
		if(convertView == null) {
			try {
				Class<? extends View> clazz = listDataSource.getListCellClass() ;
				Constructor<?> constructs = clazz.getConstructor(Context.class) ;
				convertView = (View)constructs.newInstance(getContext()) ;
				convertView.setLayoutParams(new ListView.LayoutParams(getWidth(), listRowHeight)) ;
			}
			catch(Exception e) {
			}
		}
		
		ListView.LayoutParams layoutParams = (ListView.LayoutParams)convertView.getLayoutParams() ;
		this.listDataSource.fillListViewAtIndex(position, parent, convertView, layoutParams.width, layoutParams.height) ;
		
		return convertView ;
	}

	public int getItemViewType(int position) {
		return 0;
	}

	public int getViewTypeCount() {
		return 1;
	}

	public boolean isEmpty() {
		if(listDataSource == null)
			return true ;
		return listDataSource.getListDataCount() == 0 ;
	}

	public boolean areAllItemsEnabled() {
		return true ;
	}

	public boolean isEnabled(int position) {
		return true ;
	}

	
	public int getListRowHeight() {
		return listRowHeight;
	}

	public void setListRowHeight(int listRowHeight) {
		this.listRowHeight = listRowHeight;
	}

	public EFListDataSource getListDataSource() {
		return listDataSource;
	}

	public void setListDataSource(EFListDataSource listDataSource) {
		this.listDataSource = listDataSource;
	}

}
