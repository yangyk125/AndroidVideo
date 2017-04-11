package com.android.video.views;

import java.util.HashMap;
import java.util.Map;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.ImageView;

public class EFImageButton extends ImageView {
	
	public static enum EFButtonStatus {
		BUTTON_STATUS_NORMAL ,
		BUTTON_STATUS_ACTIVE ,
		BUTTON_STATUS_SELECT ,
		BUTTON_STATUS_BUSING ,
		BUTTON_STATUS_DISABLE
	}
	
	private EFButtonStatus buttonStatus = EFButtonStatus.BUTTON_STATUS_NORMAL ;
	private Map<EFButtonStatus,Integer> statusImages = new HashMap<EFButtonStatus,Integer>();


	public EFImageButton(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
	}

	public EFImageButton(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	public EFImageButton(Context context) {
		super(context);
	}

	public void setImageResource(EFButtonStatus status, int resid) {
		statusImages.put(status, Integer.valueOf(resid)) ;
		if(status == this.buttonStatus)
			super.setImageResource(resid) ;
	}

	public int	getImageResource(EFButtonStatus status) {
		Integer idObj = statusImages.get(status);
		if(status != null)
			return idObj.intValue() ;
		return 0 ;
	}

	public EFButtonStatus getButtonStatus() {
		return this.buttonStatus;
	}

	public void setButtonStatus(EFButtonStatus buttonStatus) {
		if(this.buttonStatus != buttonStatus) {
			int resid = getImageResource(buttonStatus) ;
			if(resid > 0)
				super.setImageResource(resid) ;
		}
		this.buttonStatus = buttonStatus;
	}
	
}
