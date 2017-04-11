package com.android.video.page.widget;

import java.util.Map;

import android.content.Context;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;

import com.android.video.page.effects.EFEffectDataStructs;
import com.android.video.page.effects.EFEffectDataStructs.EFEffectDataItem;
import com.android.video.views.EFListView;
import com.android.video.views.EFListView.EFListDataSource;

public class EFEffectListView extends EFListView implements EFListDataSource {

	private EFEffectDataStructs	 effectDatas = null ;
	
	private EFEffectListView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
	}

	private EFEffectListView(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	private EFEffectListView(Context context) {
		super(context);
	}
	
	public EFEffectListView(Context context, int width) {
		super(context);
		
		this.setListDataSource(this);
		this.setListRowHeight(width/10);
	}
	
	public void initEffListView(float duration) {
		this.effectDatas = new EFEffectDataStructs(duration);
	}

	public String addEffectData(float time, Map<?,?> effectData, String cacheFolder) {
		this.effectDatas.addEffectItem(time, effectData, cacheFolder) ;
		return this.effectDatas.getStructsJson() ;
	}
	
	public String delEffectData(int effectKey) {
		this.effectDatas.delEffectItem(effectKey);
		return this.effectDatas.getStructsJson() ;
	}
	
	public void delEffectDatas() {
		this.effectDatas.delEffectItems(); 
	}

	public Class<? extends View> getListCellClass() {
		return EFEffectItemView.class ;
	}

	public int getListDataCount() {
		if(this.effectDatas != null)
			return this.effectDatas.getEffectDataItemCount() ;
		return 0;
	}

	public void fillListViewAtIndex(int index, ViewGroup parent, View cellView, int width, int height) {
		EFEffectDataItem dataItem = this.effectDatas.getEffectDataItem(index) ;
		EFEffectItemView itemView = (EFEffectItemView)cellView ;
		itemView.setEffectDataItem(dataItem);
		itemView.updateItemView(this.effectDatas.getVideoDuration(), width, height) ;
	}
	
	
}
