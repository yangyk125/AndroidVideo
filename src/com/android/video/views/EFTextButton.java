package com.android.video.views;

import java.util.HashMap;
import java.util.Map;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.TextView;

import com.android.video.views.EFImageButton.EFButtonStatus;

public class EFTextButton extends TextView {

	private EFButtonStatus buttonStatus = EFButtonStatus.BUTTON_STATUS_NORMAL ;
	private Map<EFButtonStatus,String> statusTexts = new HashMap<EFButtonStatus, String>();
	private Map<EFButtonStatus,Integer> statusColors = new HashMap<EFButtonStatus, Integer>();
	private Map<EFButtonStatus,Integer> textColors = new HashMap<EFButtonStatus, Integer>();
	
	public EFTextButton(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
	}

	public EFTextButton(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	public EFTextButton(Context context) {
		super(context);
	}

	public EFButtonStatus getButtonStatus() {
		return buttonStatus;
	}

	public void setTitle(EFButtonStatus status, String title) {
		statusTexts.put(status, title) ;
		if(status == this.buttonStatus)
			super.setText(title) ;
	}
	
	public String getTitle(EFButtonStatus status) {
		return statusTexts.get(status) ;
	}

	public void setBackgroundColor(EFButtonStatus status, int color) {
		statusColors.put(status, Integer.valueOf(color)) ;
		if(status == this.buttonStatus)
			super.setBackgroundColor(color) ;
	}
	
	public Integer getBackgroundColor(EFButtonStatus status) {
		return statusColors.get(status) ;
	}
	
	public void setTextColor(EFButtonStatus status, int color) {
		textColors.put(status, Integer.valueOf(color)) ;
		if(status == this.buttonStatus)
			super.setTextColor(color) ;
	}
	
	public Integer getTextColor(EFButtonStatus status) {
		return textColors.get(status) ;
	}
	
	public void setButtonStatus(EFButtonStatus buttonStatus) {
		
		if(this.buttonStatus != buttonStatus) {
			//background
			Integer backColor = this.getBackgroundColor(buttonStatus) ;
			if(backColor == null)
				backColor = this.getBackgroundColor(EFButtonStatus.BUTTON_STATUS_NORMAL) ;
			if(backColor == null)
				backColor = Integer.valueOf(0xFF000000) ;
			super.setBackgroundColor(backColor.intValue()) ;
			
			//text color
			Integer textColor = this.getTextColor(buttonStatus) ;
			if(textColor == null)
				textColor = this.getTextColor(EFButtonStatus.BUTTON_STATUS_NORMAL) ;
			if(textColor == null)
				textColor = Integer.valueOf(0xFFFFFFFF) ;
			super.setTextColor(textColor.intValue()) ;
			
			//text title
			String textString = this.getTitle(buttonStatus) ;
			if(textString == null)
				textString = this.getTitle(EFButtonStatus.BUTTON_STATUS_NORMAL) ;
			if(textString == null)
				textString = "" ;
			super.setText(textString) ;
		}
		
		this.buttonStatus = buttonStatus;
	}
}
