package com.android.nxui;

import java.util.List;
import java.util.Stack;

import android.content.Context;
import android.util.AttributeSet;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.Animation.AnimationListener;
import android.view.animation.TranslateAnimation;
import android.widget.FrameLayout;

public class NXPageRoot extends FrameLayout implements AnimationListener {

	private Stack<NXPageStack>		pageStacks  = new Stack<NXPageStack>() ;
	private TranslateAnimation 		animShowing = null ;
	private TranslateAnimation		animHideing = null ;
	private AlphaAnimation			animAlphing = null ;
	
	public NXPageRoot(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		initPageRoot(context, attrs) ;
	}

	public NXPageRoot(Context context, AttributeSet attrs) {
		super(context, attrs);
		initPageRoot(context, attrs) ;
	}

	public NXPageRoot(Context context) {
		super(context);
		initPageRoot(context, null) ;
	}
	
	private void initPageRoot(Context context, AttributeSet attrs) {
		this.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT)) ;
		this.animShowing = new TranslateAnimation(Animation.RELATIVE_TO_PARENT, 0.0f, Animation.RELATIVE_TO_PARENT, 0.0f, Animation.RELATIVE_TO_PARENT, 1.0f, Animation.RELATIVE_TO_PARENT, 0.0f) ;
		this.animHideing = new TranslateAnimation(Animation.RELATIVE_TO_PARENT, 0.0f, Animation.RELATIVE_TO_PARENT, 0.0f, Animation.RELATIVE_TO_PARENT, 0.0f, Animation.RELATIVE_TO_PARENT, 1.0f) ;
		this.animAlphing = new AlphaAnimation(0.0f, 1.0f);
		this.animShowing.setDuration(400) ;
		this.animHideing.setDuration(400) ;
		this.animAlphing.setDuration(50) ;
		this.animShowing.setAnimationListener(this) ;
		this.animHideing.setAnimationListener(this) ;
		this.animAlphing.setAnimationListener(this) ;
	}
	
	public NXPageStack getTopPageStack() {
		if(this.pageStacks.size() > 0)
			return this.pageStacks.lastElement() ;
		else
			return null ;
	}
	
	public NXPageStack getSecondPageStack() {
		if(this.pageStacks.size() > 1)
			return this.pageStacks.get(this.pageStacks.size()-2) ;
		else
			return null ;
	}

	public void pushPageStack(NXPageStack pageStack) {
		this.pushPageStack(pageStack, true) ;
	} 
	
	public void pushPageStack(NXPageStack pageStack, boolean animation) {
		pageStack.setPageRoot(this) ;
		this.pageStacks.push(pageStack) ;
		this.addView(pageStack) ;
		pageStack.bringToFront() ;
		pageStack.onPageStackAttached() ;
		pageStack.startAnimation(animation ? this.animShowing : this.animAlphing) ;
	}
	
	public void popPageStack(NXPageStack pageStack) {
		if(pageStack != null 
				&& this.pageStacks.size() > 1 
				&& pageStack == getTopPageStack()) {
			pageStack.startAnimation(this.animHideing) ;
		}
	}
	
	public List<NXPageStack> getAllStacks() {
		return this.pageStacks ;
	}

	//AnimationListener
	public void onAnimationStart(Animation animation) {
		NXPageStack topStack = getTopPageStack() ;
		NXPageStack secndStack = getSecondPageStack() ;
		if(animation == this.animShowing || animation == this.animAlphing) {
			if(secndStack != null)
				secndStack.onPageStackWillHide() ;
			topStack.onPageStackWillShow() ;
		}
		else if(animation == animHideing) {
			topStack.onPageStackWillHide() ;
			if(secndStack != null)
				secndStack.onPageStackWillShow() ;
		}
	}

	public void onAnimationEnd(Animation animation) {
		NXPageStack topStack = getTopPageStack() ;
		NXPageStack secndStack = getSecondPageStack() ;
		if(animation == this.animShowing || animation == this.animAlphing) {
			if(secndStack != null)
				secndStack.onPageStackDidHide() ;
			topStack.onPageStackDidShow() ;
		}
		else if(animation == animHideing) {
			topStack.onPageStackDidHide() ;
			if(secndStack != null)
				secndStack.onPageStackDidShow() ;
			
			this.pageStacks.pop() ;
			this.removeView(topStack) ;
			topStack.setPageRoot(null) ;
			topStack.onPageStackDettached() ;
		}
	}

	public void onAnimationRepeat(Animation animation) {
		
	}
	
	
}
