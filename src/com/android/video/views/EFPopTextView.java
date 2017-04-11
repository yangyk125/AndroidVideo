package com.android.video.views;

import android.content.Context;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.ViewGroup;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.Animation.AnimationListener;
import android.view.animation.AnimationSet;
import android.view.animation.ScaleAnimation;
import android.widget.AbsoluteLayout;
import android.widget.TextView;

@SuppressWarnings("deprecation")
public class EFPopTextView extends TextView implements AnimationListener {
	
	private EFPageView.LayoutParams layoutParams = null ;
	
	private AnimationSet	animGroup   = null ;
	private Animation		scaleBigger = null ;
	private Animation		scaleSmallr = null ;
	private Animation		scaleAlpha  = null ;
	
	private static EFPopTextView staticPopView = null ;
	
	public EFPopTextView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		initPopTextView();
	}

	public EFPopTextView(Context context, AttributeSet attrs) {
		super(context, attrs);
		initPopTextView();
	}

	public EFPopTextView(Context context) {
		super(context);
		initPopTextView();
	}
	
	private void initPopTextView() {
		this.layoutParams = new EFPageView.LayoutParams(0,0,0,0);
		this.animGroup    = new AnimationSet(getContext(), null);
		this.scaleBigger  = new ScaleAnimation(1.0f, 1.1f, 1.0f, 1.1f, Animation.RELATIVE_TO_SELF, 0.5f, Animation.RELATIVE_TO_SELF, 0.5f);
		this.scaleSmallr  = new ScaleAnimation(1.1f, 0.6f, 1.1f, 0.6f, Animation.RELATIVE_TO_SELF, 0.5f, Animation.RELATIVE_TO_SELF, 0.5f);
		this.scaleAlpha   = new AlphaAnimation(1.0f, 0.0f) ;
		this.animGroup.addAnimation(this.scaleBigger) ;
		this.animGroup.addAnimation(this.scaleSmallr) ;
		this.animGroup.addAnimation(this.scaleAlpha) ;
		this.animGroup.setAnimationListener(this) ;
	}
	
	
	public void onAnimationStart(Animation animation) {
		
	}

	public void onAnimationEnd(Animation animation) {
		ViewGroup parent = (ViewGroup)this.getParent() ;
		if(parent != null)
			parent.removeView(this) ;
	}

	public void onAnimationRepeat(Animation animation) {
		
	}

	public static void showPopTextView(Context context, AbsoluteLayout parent, String text, float textPixels, int textColor, float duration)  {
		
		if(staticPopView == null) 
			staticPopView = new EFPopTextView(context);
		
		staticPopView.clearAnimation() ;
		staticPopView.animGroup.cancel() ;
		staticPopView.animGroup.reset() ;
		
		ViewGroup lastParent = (ViewGroup)staticPopView.getParent() ;
		if(lastParent != null) 
			lastParent.removeView(staticPopView) ;
		
		staticPopView.setText(text) ;
		staticPopView.setGravity(Gravity.CENTER_HORIZONTAL|Gravity.CENTER_VERTICAL) ;
		staticPopView.setTextSize(TypedValue.COMPLEX_UNIT_PX, textPixels) ;
		staticPopView.setTextColor(textColor) ;
		
		int textWidth = (int)staticPopView.getPaint().measureText(staticPopView.getText().toString()) + 2 ;
		int textHeight = (int)(textPixels * 1.2f) ;
		
		int pareWidth = parent.getWidth() ;
		int pareHeight = parent.getHeight() ;
		
		staticPopView.layoutParams.x = (pareWidth-textWidth)/2 ;
		staticPopView.layoutParams.y = (pareHeight-textHeight)/2 ;
		staticPopView.layoutParams.width = textWidth ;
		staticPopView.layoutParams.height = textHeight ;
		
		staticPopView.setLayoutParams(staticPopView.layoutParams) ;
		parent.addView(staticPopView) ;
		
		//
		staticPopView.scaleBigger.setStartOffset(0) ;
		staticPopView.scaleBigger.setDuration((long)(duration*200)) ;
		
		staticPopView.scaleSmallr.setStartOffset((long)(duration*200)) ;
		staticPopView.scaleAlpha.setStartOffset((long)(duration*200)) ;
		
		staticPopView.scaleSmallr.setDuration((long)(duration*800)) ;
		staticPopView.scaleAlpha.setDuration((long)(duration*800)) ;
		
		staticPopView.startAnimation(staticPopView.animGroup) ;
	}
	
}
