package com.android.video.page.widget;

import java.util.Map;

import android.content.Context;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.View;
import android.widget.AbsoluteLayout;
import android.widget.ImageView.ScaleType;
import android.widget.TextView;

import com.android.video.https.EFDownloadManager;
import com.android.video.https.EFEffectsManager;
import com.android.video.https.EFEffectsManager.EFEffectsListener;
import com.android.video.views.EFImageButton.EFButtonStatus;
import com.android.video.views.EFImageView;
import com.android.video.views.EFTextButton;

@SuppressWarnings("deprecation")
public class EFEffectOnlineView extends AbsoluteLayout implements View.OnClickListener, EFEffectsListener{

	public static interface EffectItemListener {
		public void onSelectEffectData(Map<?,?> effectData) ;
	}
	
	private Map<?, ?> 		effectData	 = null ;
	
	private TextView		effectName	= null ;
	private EFImageView	  	effectIcon  = null ;
	private EFTextButton	effectDown  = null ;
	
	private EffectItemListener	effectItemListener = null ;

	public EFEffectOnlineView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		initEffectItemView() ;
	}

	public EFEffectOnlineView(Context context, AttributeSet attrs) {
		super(context, attrs);
		initEffectItemView() ;
	}

	public EFEffectOnlineView(Context context) {
		super(context);
		initEffectItemView() ;
	}

	protected void initEffectItemView() {
		
		this.effectName = new TextView(getContext());
		this.effectName.setLines(1) ; 
		this.effectName.setGravity(Gravity.CENTER_VERTICAL|Gravity.LEFT) ;
		this.effectName.setLayoutParams(new AbsoluteLayout.LayoutParams(0,0,0,0)) ;
		this.effectName.setTextColor(0xFF0000BB) ;
		this.addView(this.effectName) ;
		
		this.effectIcon = new EFImageView(getContext());
		this.effectIcon.setScaleType(ScaleType.CENTER_INSIDE) ;
		this.effectIcon.setLayoutParams(new AbsoluteLayout.LayoutParams(0,0,0,0)) ;
		this.addView(this.effectIcon) ;
		
		this.effectDown = new EFTextButton(getContext());
		this.effectDown.setGravity(Gravity.CENTER_VERTICAL|Gravity.LEFT) ;
		this.effectDown.setLayoutParams(new AbsoluteLayout.LayoutParams(0,0,0,0)) ;
		this.effectDown.setOnClickListener(this) ;
		this.effectDown.setBackgroundColor(EFButtonStatus.BUTTON_STATUS_NORMAL, 0xFF00AA00) ;
		this.effectDown.setBackgroundColor(EFButtonStatus.BUTTON_STATUS_BUSING, 0xFF999999) ;
		this.effectDown.setTextColor(EFButtonStatus.BUTTON_STATUS_NORMAL, 0xFFFFFFFF) ;
		this.effectDown.setTextColor(EFButtonStatus.BUTTON_STATUS_BUSING, 0xFFBBBBBB) ;
		this.addView(this.effectDown) ;
	}
	
	
	public Map<?, ?> getEffectData() {
		return effectData;
	}

	public void setEffectData(Map<?, ?> effectData) {
		this.effectData = effectData;
	}

	public void updateEffectItem(int width, int height) {
		
		String effName = (String)effectData.get("effect_name") ;
		String icnFile = (String)effectData.get("effect_icon") ;
		String cfgFile = (String)effectData.get("config") ;
		
		this.effectName.setText(effName) ;
		
		EFDownloadManager.getShareManager().fetchHttpFile(icnFile, this.effectIcon) ;
		
		if(EFEffectsManager.getShareManager().isEffectCached(cfgFile))
			this.effectDown.setTitle(EFButtonStatus.BUTTON_STATUS_NORMAL, "使用") ;
		else 
			this.effectDown.setTitle(EFButtonStatus.BUTTON_STATUS_NORMAL, "下载") ;
		
		if(this.effectIcon.getPaddingLeft() == 0) {
			
			this.effectIcon.setPadding(height/15, height/15, height/15, height/15) ;
			AbsoluteLayout.LayoutParams params = (AbsoluteLayout.LayoutParams)this.effectIcon.getLayoutParams() ;
			params.x = params.y = 0 ;
			params.width = params.height = height ;
			
			this.effectName.setPadding(height/15, 0, 0, 0) ;
			this.effectName.setTextSize(TypedValue.COMPLEX_UNIT_PX, height/12);
			
			params = (AbsoluteLayout.LayoutParams)this.effectName.getLayoutParams() ;
			params.x = height ;
			params.y = 0 ;
			params.width = width - height ;
			params.height = height*2/5 ;
			
			this.effectDown.setPadding(height/15, 0, 0, 0) ;
			this.effectDown.setTextSize(TypedValue.COMPLEX_UNIT_PX, height/12);
			params = (AbsoluteLayout.LayoutParams)this.effectDown.getLayoutParams() ;
			params.x = height ;
			params.y = height*2/5 ;
			params.width = width - height ;
			params.height = height*3/5 ;
			
			this.requestLayout() ;
		}
	}

	public EffectItemListener getEffectItemListener() {
		return effectItemListener;
	}

	public void setEffectItemListener(EffectItemListener efefctItemListener) {
		this.effectItemListener = efefctItemListener;
	}

	public void onClick(View view) {
		if(view == this.effectDown) {
			if(this.effectDown.getButtonStatus() == EFButtonStatus.BUTTON_STATUS_BUSING)
				return ;
			String effFile = (String)effectData.get("config") ;
			if(EFEffectsManager.getShareManager().isEffectCached(effFile)) {
				if(this.effectItemListener != null)
					this.effectItemListener.onSelectEffectData(effectData) ;
			}
			else {
				EFEffectsManager.getShareManager().fetchEffectFile(effFile, this) ;
			}
		}
	}
	
	public void onEffectFileDownloadReady(String effectFile) {
		this.effectDown.setButtonStatus(EFButtonStatus.BUTTON_STATUS_BUSING) ;
		this.effectDown.setText("准备下载...") ;
	}

	public void onEffectFileDownloadStart(String effectFile) {
		this.effectDown.setButtonStatus(EFButtonStatus.BUTTON_STATUS_BUSING) ;
		this.effectDown.setText(String.format("正在下载(%.1f%%)...", 0.0f)) ;
	}

	public void onEffectFileDownloadProgress(String effectFile, float progress) {
		this.effectDown.setText(String.format("正在下载(%.1f%%)...", progress*100)) ;
	}

	public void onEffectFileDownloadFailure(String effectFile) {
		this.effectDown.setTitle(EFButtonStatus.BUTTON_STATUS_NORMAL, "下载") ;
		this.effectDown.setButtonStatus(EFButtonStatus.BUTTON_STATUS_NORMAL) ;
	}

	public void onEffectFileDownloadComplete(String effectFile, String cacheFolder) {
		this.effectDown.setTitle(EFButtonStatus.BUTTON_STATUS_NORMAL, "使用") ;
		this.effectDown.setButtonStatus(EFButtonStatus.BUTTON_STATUS_NORMAL) ;
	}
	
}
