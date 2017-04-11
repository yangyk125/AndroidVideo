package com.android.nxui;

import android.app.Activity;
import android.content.Context;
import android.graphics.Rect;
import android.os.Handler;
import android.util.AttributeSet;
import android.util.Log;
import android.view.animation.Animation;
import android.view.animation.Animation.AnimationListener;
import android.view.animation.TranslateAnimation;
import android.widget.AbsoluteLayout;

@SuppressWarnings("deprecation")
public abstract class NXPageView extends AbsoluteLayout implements Handler.Callback {

	protected NXPageStack		pageStack   = null ;
	protected Handler			pageHandler = null ;
	
	protected Rect				screenRect  = null ;
	protected Rect				headerRect  = null ;
	protected Rect				clientRect  = null ;
	
	protected NXPageView		popingPage  = null ;
	protected Animation			animShowing = null ;
	protected Animation			animHideing = null ;
	protected NXPageView.LayoutParams layoutParam = null ;
	
	protected long				pageShows   = 0L  ;

	public NXPageView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		initPageView() ;
	}

	public NXPageView(Context context, AttributeSet attrs) {
		super(context, attrs);
		initPageView() ;
	}

	public NXPageView(Context context) {
		super(context);
		initPageView() ;
	}
	
	private void initPageView() {
		
		this.screenRect = new Rect();
		
		Activity activity = (Activity)this.getContext() ;
		activity.getWindowManager().getDefaultDisplay().getRectSize(this.screenRect) ;

		int statusBarHeight = 0 ;
		int resid = activity.getResources().getIdentifier("status_bar_height", "dimen", "android") ;
		if(resid > 0)
			statusBarHeight = activity.getResources().getDimensionPixelSize(resid) ;
		
		this.clientRect = new Rect(this.screenRect);
		this.clientRect.bottom = this.screenRect.bottom - statusBarHeight ;
		
		this.headerRect = new Rect(this.screenRect) ;
		this.headerRect.bottom = this.screenRect.width() / 10 ;
		
		this.pageHandler = new Handler(this); 
	}
	
	public NXPageStack getPageStack() {
		return pageStack;
	}

	public void setPageStack(NXPageStack pageStack) {
		this.pageStack = pageStack;
	}
	
	public Rect getClientRect() {
		return clientRect;
	}

	public Rect getScreenRect() {
		return screenRect;
	}
	
	public Handler getPageHandler() {
		return pageHandler;
	}
	
	public void popFromParent() {
		this.pageStack.popPageView(this) ;
	}
	
	private AnimationListener popAnimListener = new AnimationListener() {
		public void onAnimationStart(Animation animation) {
			
		}
		public void onAnimationEnd(Animation animation) {
			if(animation == animShowing) {
				popingPage.onPageViewDidShow() ;
			}
			else if(animation == animHideing) {
				popingPage.onPageViewDidHide() ;
				removeView(popingPage);
				popingPage.onPageViewDettached() ;
				popingPage = null ;
			}
		}
		public void onAnimationRepeat(Animation animation) {

		}
	} ;
	
	public void showPopupPage(NXPageView pageView) {
		
		if(this.popingPage != null)
			return ;
		
		this.popingPage = pageView ;
		
		if(this.animShowing == null) {
			this.animShowing = new TranslateAnimation(Animation.RELATIVE_TO_PARENT, 0.0f, Animation.RELATIVE_TO_PARENT, 0.0f, Animation.RELATIVE_TO_PARENT, 1.0f, Animation.RELATIVE_TO_PARENT, 0.0f) ;
			this.animShowing.setDuration(300) ;
			this.animShowing.setAnimationListener(this.popAnimListener) ;
		}
		
		if(this.animHideing == null) {
			this.animHideing = new TranslateAnimation(Animation.RELATIVE_TO_PARENT, 0.0f, Animation.RELATIVE_TO_PARENT, 0.0f, Animation.RELATIVE_TO_PARENT, 0.0f, Animation.RELATIVE_TO_PARENT, 1.0f) ;
			this.animHideing.setDuration(300) ;
			this.animHideing.setAnimationListener(this.popAnimListener) ;
		}
		
		if(this.layoutParam == null) {
			this.layoutParam = new NXPageView.LayoutParams(this.getWidth(), this.getHeight(), 0, 0) ;
		}
		
		this.popingPage.setLayoutParams(this.layoutParam) ;
		
		this.addView(this.popingPage) ;
		
		this.popingPage.onPageViewAttached() ;
		this.popingPage.onPageViewWillShow() ;
		
		this.bringChildToFront(this.popingPage) ;
		
		this.popingPage.startAnimation(this.animShowing) ;
	}
	
	public void hidePopupPage(NXPageView pageView) {
		if(this.popingPage == pageView) {
			this.popingPage.onPageViewWillHide() ;
			this.popingPage.startAnimation(this.animHideing) ;
		}
	}
	
	public void hideSelfPopupPage() {
		NXPageView parentPage = (NXPageView)this.getParent() ;
		if(parentPage != null)
			parentPage.hidePopupPage(this) ;
	}
		
	public long getPageShows() {
		return this.pageShows;
	}

	public void onPageViewAttached() {
		Log.d("PAGE", getPageName() + "->onPageViewAttached()") ;
	}
	
	public void onPageViewDettached() {
		Log.d("PAGE", getPageName() + "->onPageViewDettached()") ;
	}

	public void onPageViewWillShow() {
		Log.d("PAGE", getPageName() + "->onPageViewWillShow()") ;
	}
	
	public void onPageViewDidShow() {
		pageShows ++ ;
		Log.d("PAGE", getPageName() + "->onPageViewDidShow()") ;
	}

	public void onPageViewWillHide() {
		Log.d("PAGE", getPageName() + "->onPageViewWillHide()") ;
	}

	public void onPageViewDidHide() {
		Log.d("PAGE", getPageName() + "->onPageViewDidHide()") ;
	}
	
	public abstract String getPageName() ;
}
