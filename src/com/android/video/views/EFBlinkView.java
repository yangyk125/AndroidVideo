package com.android.video.views;

import android.content.Context;
import android.util.AttributeSet;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.widget.ImageView;

public class EFBlinkView extends ImageView  {
	
	private AlphaAnimation  blinkAnimation 			= null ;
	
	public EFBlinkView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		initBlinkView() ;
	}

	public EFBlinkView(Context context, AttributeSet attrs) {
		super(context, attrs);
		initBlinkView() ;
	}

	public EFBlinkView(Context context) {
		super(context);
		initBlinkView() ;
	}

	private void initBlinkView() {
		this.blinkAnimation = new AlphaAnimation(1, 0);
		this.blinkAnimation.setRepeatCount(Animation.INFINITE) ;
		this.blinkAnimation.setRepeatMode(Animation.REVERSE) ;
	}
	
	public void setBlinkFreq(float frequency) {
		this.blinkAnimation.setDuration((long)(500/frequency)) ;
	}
	
	public void startBlink() {
		this.startAnimation(this.blinkAnimation) ;
	}
	
	public void stopBlink() {
		this.blinkAnimation.cancel() ;
		this.blinkAnimation.reset() ;
		this.clearAnimation() ; 
	}
}
