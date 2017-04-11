package com.android.nxui;

import java.util.Stack;

import android.content.Context;
import android.util.AttributeSet;
import android.view.animation.Animation;
import android.view.animation.Animation.AnimationListener;
import android.view.animation.TranslateAnimation;
import android.widget.FrameLayout;

public class NXPageStack extends FrameLayout implements AnimationListener {

	private NXPageRoot		   		pageRoot    = null ;
	private Stack<NXPageView>  		pageViews   = new Stack<NXPageView>() ;
	private TranslateAnimation 		animShowing = null ;
	private TranslateAnimation		animHideing = null ;
	
	protected NXPageStack(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
	}

	protected NXPageStack(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	protected NXPageStack(Context context) {
		super(context);
	}
	
	public NXPageStack(NXPageView rootPage, Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		initPageStack(rootPage, context, attrs);
	}

	public NXPageStack(NXPageView rootPage, Context context, AttributeSet attrs) {
		super(context, attrs);
		initPageStack(rootPage, context, attrs);
	}

	public NXPageStack(NXPageView rootPage, Context context) {
		super(context);
		initPageStack(rootPage, context, null);
	}
	
	private void initPageStack(NXPageView rootPage, Context context, AttributeSet attrs) {
		
		this.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT)) ;
		
		this.animShowing = new TranslateAnimation(Animation.RELATIVE_TO_PARENT, 1.0f, Animation.RELATIVE_TO_PARENT, 0.0f, Animation.RELATIVE_TO_PARENT, 0.0f, Animation.RELATIVE_TO_PARENT, 0.0f) ;
		this.animHideing = new TranslateAnimation(Animation.RELATIVE_TO_PARENT, 0.0f, Animation.RELATIVE_TO_PARENT, 1.0f, Animation.RELATIVE_TO_PARENT, 0.0f, Animation.RELATIVE_TO_PARENT, 0.0f) ;
		
		this.animShowing.setDuration(400) ;
		this.animHideing.setDuration(400) ;
		
		this.animShowing.setAnimationListener(this) ;
		this.animHideing.setAnimationListener(this) ;

		this.addView(rootPage) ;
		pageViews.push(rootPage) ;
		rootPage.setPageStack(this) ;
	}

	public NXPageRoot getPageRoot() {
		return pageRoot;
	}

	public void setPageRoot(NXPageRoot pageRoot) {
		this.pageRoot = pageRoot;
	}
	
	public NXPageView getTopPageView() {
		if(this.pageViews.size() > 0)
			return this.pageViews.lastElement() ;
		else
			return null ;
	}
	
	public NXPageView getSecondPageView() {
		if(this.pageViews.size() > 1)
			return this.pageViews.get(this.pageViews.size()-2) ;
		else
			return null ;
	}

	public void pushPageView(NXPageView pageView) {
		pageView.setPageStack(this) ;
		this.pageViews.push(pageView) ;
		this.addView(pageView) ;
		pageView.bringToFront() ;
		pageView.onPageViewAttached() ;
		pageView.startAnimation(this.animShowing) ;
	}
	
	public void popPageView(NXPageView pageView) {
		if(pageView != null  && pageView == getTopPageView()) {
			if(this.pageViews.size() > 1)
				pageView.startAnimation(this.animHideing) ;
			else 
				this.popFromParent() ;
		}
	}
	
	public void popFromParent() {
		this.pageRoot.popPageStack(this) ;
	}

	public void onAnimationStart(Animation animation) {
		NXPageView topView = getTopPageView() ;
		NXPageView secndView = getSecondPageView() ;
		if(animation == this.animShowing) {
			if(secndView != null)
				secndView.onPageViewWillHide() ;
			topView.onPageViewWillShow();
		}
		else if(animation == this.animHideing) {
			topView.onPageViewWillHide();
			if(secndView != null)
				secndView.onPageViewWillShow() ;
		}
	}

	public void onAnimationEnd(Animation animation) {
		NXPageView topView = getTopPageView() ;
		NXPageView secndView = getSecondPageView() ;
		if(animation == this.animShowing) {
			if(secndView != null)
				secndView.onPageViewDidHide();
			topView.onPageViewDidShow();
		}
		else if(animation == this.animHideing) {
			topView.onPageViewDidHide();
			if(secndView != null)
				secndView.onPageViewDidShow();
			
			this.pageViews.pop() ;
			this.removeView(topView) ;
			topView.setPageStack(null) ;
			topView.onPageViewDettached() ;
		}
	}

	public void onAnimationRepeat(Animation animation) {

	}
	
	protected void onPageStackAttached() {
		for(NXPageView pageView : this.pageViews) {
			pageView.onPageViewAttached() ;
		}
	}
	
	protected void onPageStackDettached() {
		for(NXPageView pageView : this.pageViews) {
			pageView.onPageViewDettached() ;
		}
	}

	protected void onPageStackWillShow() {
		NXPageView topView = getTopPageView() ;
		if(topView != null)
			topView.onPageViewWillShow() ;
	}
	
	protected void onPageStackDidShow() {
		NXPageView topView = getTopPageView() ;
		if(topView != null)
			topView.onPageViewDidShow() ;
	}

	protected void onPageStackWillHide() {
		NXPageView topView = getTopPageView() ;
		if(topView != null)
			topView.onPageViewWillHide() ;
	}

	protected void onPageStackDidHide() {
		NXPageView topView = getTopPageView() ;
		if(topView != null)
			topView.onPageViewDidHide() ;
	}
}
