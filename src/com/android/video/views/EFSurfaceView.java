package com.android.video.views;

import android.content.Context;
import android.util.AttributeSet;
import android.view.SurfaceView;

public class EFSurfaceView extends SurfaceView {

	public EFSurfaceView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		initSurfaceView();
	}

	public EFSurfaceView(Context context, AttributeSet attrs) {
		super(context, attrs);
		initSurfaceView();
	}

	public EFSurfaceView(Context context) {
		super(context);
		initSurfaceView();
	}
	
	private void initSurfaceView(){
		
	}
}
