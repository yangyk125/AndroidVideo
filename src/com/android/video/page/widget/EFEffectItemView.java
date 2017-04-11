package com.android.video.page.widget;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.AbsoluteLayout;
import android.widget.ImageView.ScaleType;

import com.android.video.R;
import com.android.video.https.EFDownloadManager;
import com.android.video.page.effects.EFEffectDataStructs.EFEffectDataItem;
import com.android.video.tools.EFBitmapManager;
import com.android.video.views.EFImageView;

@SuppressWarnings("deprecation")
public class EFEffectItemView extends AbsoluteLayout {

	private EFEffectDataItem	effectDataItem = null ;
	
	private EFImageView			iconView  = null ;
	private EFImageView			barsLeft  = null ;
	private EFImageView			barsCeter = null ;
	private EFImageView			barsRight = null ;
	

	public EFEffectItemView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		initEffectItemView() ;
	}

	public EFEffectItemView(Context context, AttributeSet attrs) {
		super(context, attrs);
		initEffectItemView() ;
	}

	public EFEffectItemView(Context context) {
		super(context);
		initEffectItemView() ;
	}

	protected void initEffectItemView() {
		
		this.iconView = new EFImageView(getContext()) ;
		this.barsLeft = new EFImageView(getContext()) ;
		this.barsCeter = new EFImageView(getContext()) ;
		this.barsRight = new EFImageView(getContext()) ;
		
		this.iconView.setScaleType(ScaleType.CENTER_INSIDE);
		this.barsLeft.setScaleType(ScaleType.CENTER_CROP);
		this.barsRight.setScaleType(ScaleType.CENTER_CROP);
		this.barsCeter.setScaleType(ScaleType.CENTER_INSIDE);
		
		this.iconView.setLayoutParams(new AbsoluteLayout.LayoutParams(0, 0, 0, 0));
		this.barsLeft.setLayoutParams(new AbsoluteLayout.LayoutParams(0, 0, 0, 0));
		this.barsRight.setLayoutParams(new AbsoluteLayout.LayoutParams(0, 0, 0, 0));
		this.barsCeter.setLayoutParams(new AbsoluteLayout.LayoutParams(0, 0, 0, 0));
		
		this.barsLeft.setImageResource(R.drawable.icon_effect_left);
		this.barsRight.setImageResource(R.drawable.icon_effect_right);
		this.barsCeter.setBackgroundColor(0xFF465051);
		
		this.addView(this.iconView);
		this.addView(this.barsLeft);
		this.addView(this.barsRight);
		this.addView(this.barsCeter);
	}
	
	public EFEffectDataItem getEffectDataItem() {
		return effectDataItem;
	}

	public void setEffectDataItem(EFEffectDataItem effectDataItem) {
		this.effectDataItem = effectDataItem;
		String effectIcon = effectDataItem.getEffectIcon() ;
		if(effectIcon != null && effectIcon.startsWith("/")) 
			this.iconView.setImageBitmap(EFBitmapManager.loadBitmapFromFile(effectIcon));
		else if(effectIcon != null && effectIcon.startsWith("http")) 
			EFDownloadManager.getShareManager().fetchHttpFile(effectIcon, this.iconView);
	}
	
	public void updateItemView(float duration, int width, int height) 
	{
		int progsWidth = (int)(this.effectDataItem.getEffectDuration()/duration*(width-height)) ;
		int progsLeft  = (int)(this.effectDataItem.getEffectStart()/duration*(width-height)) ;
		
		AbsoluteLayout.LayoutParams layoutParam = (AbsoluteLayout.LayoutParams)this.iconView.getLayoutParams() ;
		layoutParam.x = 0 ;
		layoutParam.y = 0 ;
		layoutParam.width = height/2 ;
		layoutParam.height = height ;
		
		layoutParam = (AbsoluteLayout.LayoutParams)this.barsLeft.getLayoutParams() ;
		layoutParam.x = height/2 + progsLeft ;
		layoutParam.y = 0 ;
		layoutParam.width = height/2 ;
		layoutParam.height = height ;
		
		layoutParam = (AbsoluteLayout.LayoutParams)this.barsCeter.getLayoutParams() ;
		layoutParam.x = height/2 + progsLeft + height/2 ;
		layoutParam.y = 0 ;
		layoutParam.width = progsWidth-height ;
		layoutParam.height = height ;
		
		layoutParam = (AbsoluteLayout.LayoutParams)this.barsRight.getLayoutParams() ;
		layoutParam.x = height/2 + progsLeft + progsWidth - height/2 ;
		layoutParam.y = 0 ;
		layoutParam.width = height/2 ;
		layoutParam.height = height ;

		this.requestLayout(); 
	}
}
