package com.android.video.views;

import android.app.Activity;
import android.content.Context;
import android.graphics.Rect;
import android.os.Handler;
import android.util.AttributeSet;
import android.view.animation.Animation;
import android.view.animation.Animation.AnimationListener;
import android.view.animation.TranslateAnimation;
import android.widget.AbsoluteLayout;

@SuppressWarnings("deprecation")
public abstract class EFPageView extends AbsoluteLayout implements Handler.Callback {

	protected Handler			pageHandler = null ;
	
	protected Rect				screenRect  = null ;
	protected Rect				headerRect  = null ;
	protected Rect				clientRect  = null ;
	
	protected EFPageView		popingPage  = null ;
	protected Animation			animShowing = null ;
	protected Animation			animHideing = null ;
	protected EFPageView.LayoutParams layoutParam = null ;
		
	public EFPageView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		initPageView() ;
	}

	public EFPageView(Context context, AttributeSet attrs) {
		super(context, attrs);
		initPageView() ;
	}

	public EFPageView(Context context) {
		super(context);
		initPageView() ;
	}
	
	protected void initPageView() {
		
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
		this.headerRect.bottom = this.screenRect.width() / 8 ;
		
		this.pageHandler = new Handler(this); 
	}

	public Rect getClientRect() {
		return clientRect;
	}

	public Rect getHeaderRect() {
		return headerRect;
	}

	public Rect getScreenRect() {
		return screenRect;
	}

	public Handler getPageHandler() {
		return pageHandler;
	}
	
	private AnimationListener popAnimListener = new AnimationListener() {
		public void onAnimationStart(Animation animation) {
			
		}
		public void onAnimationEnd(Animation animation) {
			if(animation == animShowing) {
				popingPage.pageDonePopShow() ;
			}
			else if(animation == animHideing) {
				popingPage.pageDonePopHide() ;
				removeView(popingPage);
				popingPage = null ;
			}
		}
		public void onAnimationRepeat(Animation animation) {

		}
	} ;
	
	public void showPopupPage(EFPageView pageView) {
		
		if(this.popingPage != null)
			return ;
		
		this.popingPage = pageView ;
		this.popingPage.pageWillPopShow() ;
		
		if(this.animShowing == null) {
			this.animShowing = new TranslateAnimation(Animation.RELATIVE_TO_PARENT, 0.0f, Animation.RELATIVE_TO_PARENT, 0.0f, Animation.RELATIVE_TO_PARENT, 1.0f, Animation.RELATIVE_TO_PARENT, 0.0f) ;
			this.animShowing.setDuration(400) ;
			this.animShowing.setAnimationListener(this.popAnimListener) ;
		}
		
		if(this.animHideing == null) {
			this.animHideing = new TranslateAnimation(Animation.RELATIVE_TO_PARENT, 0.0f, Animation.RELATIVE_TO_PARENT, 0.0f, Animation.RELATIVE_TO_PARENT, 0.0f, Animation.RELATIVE_TO_PARENT, 1.0f) ;
			this.animHideing.setDuration(400) ;
			this.animHideing.setAnimationListener(this.popAnimListener) ;
		}
		
		if(this.layoutParam == null) {
			this.layoutParam = new EFPageView.LayoutParams(this.getWidth(), this.getHeight(), 0, 0) ;
		}
		
		this.popingPage.setLayoutParams(this.layoutParam) ;
		
		this.addView(this.popingPage) ;
		this.bringChildToFront(this.popingPage) ;
		
		this.popingPage.startAnimation(this.animShowing) ;
	}
	
	public void hidePopupPage(EFPageView pageView) {
		if(this.popingPage == pageView) {
			this.popingPage.pageWillPopHide() ;
			this.popingPage.startAnimation(this.animHideing) ;
		}
	}
	
	public void hideSelfPopupPage() {
		EFPageView parentPage = (EFPageView)this.getParent() ;
		if(parentPage != null)
			parentPage.hidePopupPage(this) ;
	}
		
	public void pageWillPopShow() {} ;
	public void pageDonePopShow() {} ;
	public void pageWillPopHide() {} ;
	public void pageDonePopHide() {} ;
	
}
