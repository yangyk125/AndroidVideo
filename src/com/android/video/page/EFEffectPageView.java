package com.android.video.page;

import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.util.List;
import java.util.Map;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Message;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;

import com.android.video.https.EFEffectsManager;
import com.android.video.https.EFRequestsManager.EFRequest;
import com.android.video.https.EFRequestsManager.EFRequestsListener;
import com.android.video.page.widget.EFEffectOnlineView;
import com.android.video.page.widget.EFEffectOnlineView.EffectItemListener;
import com.android.video.tools.E4FunTool;
import com.android.video.views.EFListView;
import com.android.video.views.EFListView.EFListDataSource;
import com.android.video.views.EFPageView;
import com.android.nxui.NXPageView;

@SuppressWarnings("deprecation")
public class EFEffectPageView extends NXPageView implements EFRequestsListener, EFListDataSource, EffectItemListener {

	public static interface EFEffectPageListener {
		public void onEffectSelectConfirm(EFEffectPageView effectPage, Map<?, ?> effectData, String effectCache) ;
		public void onEffectSelectCancel(EFEffectPageView effectPage) ;
	}
	
	protected EFEffectPageListener	effectListener = null ;
	
	private EFListView  		tableListView = null ;
	private List<?>				tableListData = null ;
	
	
	public EFEffectPageView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		initEffectPageView() ;
	}

	public EFEffectPageView(Context context, AttributeSet attrs) {
		super(context, attrs);
		initEffectPageView() ;
	}

	public EFEffectPageView(Context context) {
		super(context);
		initEffectPageView() ;
	}
	
	protected void initEffectPageView() {
		
		this.setBackgroundColor(0xFF444444) ;
		
		this.tableListView = new EFListView(getContext()) ;
		this.tableListView.setLayoutParams(new EFPageView.LayoutParams(screenRect.width(), screenRect.height(), 0, 0)) ;
		this.tableListView.setListRowHeight(screenRect.width()/2) ;
		this.tableListView.setListDataSource(this) ;
		this.addView(this.tableListView) ;
	}
	
	
	public boolean handleMessage(Message msg) {

		return true ;
	}
	
	public void onPageViewWillShow() {
		super.onPageViewWillShow();
	}

	public void onPageViewDidShow() {
		super.onPageViewDidShow();
		
		/*
		EFRequest request = EFRequestsManager.buildRequest(String.format("%s/effects", EFRequestsManager.getHttpServer()), EFRequest.GET) ;
		request.setListener(this) ;
		EFRequestsManager.getShareManager().addRequest(request) ;
		*/
		
		this.onRequestComplete(null);
	}

	public void onRequestStart(EFRequest request) {
		
	}

	public void onRequestFailure(EFRequest request) {

	}

	public void onRequestComplete(EFRequest request) {
		//String content = request.getContent() ;
		String content = null ;
		AssetManager assetManager = this.getContext().getAssets() ;
		try {
			String contentFile = "e4fun/https/http_content.txt" ;
			InputStream in = assetManager.open(contentFile) ;
			ByteArrayOutputStream out = new ByteArrayOutputStream();
			byte[] buf = new byte[128] ;
			int len = 0 ;
			while((len = in.read(buf, 0, buf.length)) > 0)
				out.write(buf, 0, len);
			content = new String(out.toByteArray(), "utf8");
		}
		catch(Exception e) {
			e.printStackTrace(); 
		}
		this.tableListData = (List<?>)E4FunTool.parseJson(content) ;
		this.tableListView.requestLayout() ;
	}
	
	public Class<? extends View> getListCellClass() {
		return EFEffectOnlineView.class ;
	}

	public int getListDataCount() {
		if(this.tableListData == null)
			return 0 ;
		return this.tableListData.size() ;
	}

	public void fillListViewAtIndex(int index, ViewGroup parent, View cellView, int width, int height) {
		
		Map<?, ?> effectData = (Map<?, ?>)this.tableListData.get(index) ;
		
		EFEffectOnlineView effectItem = (EFEffectOnlineView)cellView ;
		effectItem.setBackgroundColor(0xFFFFFFFF) ;
		effectItem.setEffectData(effectData) ;
		effectItem.setEffectItemListener(this) ;
		
		effectItem.updateEffectItem(width, height) ;
	}

	public EFEffectPageListener getEffectListener() {
		return effectListener;
	}

	public void setEffectListener(EFEffectPageListener effectListener) {
		this.effectListener = effectListener;
	}

	public void onSelectEffectData(Map<?, ?> effectData) {
		if(this.effectListener != null) {
			String effectUrl = (String)effectData.get("config") ;
			String effectFolder = EFEffectsManager.getShareManager().getEffectFolder(effectUrl) ;
			this.effectListener.onEffectSelectConfirm(this, effectData, effectFolder) ;
		}
	}
	
	public String getPageName() {

		return "EffectListPage";
	}
}
