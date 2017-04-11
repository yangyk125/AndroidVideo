package com.android.video.views;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Paint.Align;
import android.util.AttributeSet;
import android.view.View;

public class EFProgressView extends View {

	private Paint 	textPaint = new Paint();
	private float	progress  = 0.0f ;
	
	private EFProgressView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
	}

	private EFProgressView(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	private EFProgressView(Context context) {
		super(context);
	}
	
	public EFProgressView(Context context, float textSize) {
		super(context) ;
		
		this.setBackgroundColor(0xAB000000) ;
		this.textPaint.setTextAlign(Align.CENTER) ;
		this.textPaint.setColor(0xFFEEEEEE) ;
		this.textPaint.setTextSize(textSize) ;
		this.textPaint.setAntiAlias(true) ; // 抗锯齿
	}

	public void draw(Canvas canvas) {
		super.draw(canvas);
		canvas.drawText(String.format("%d%%", (int)(progress*100)), this.getWidth()/2, this.getHeight()/2, this.textPaint) ;
	}

	public float getProgress() {
		return progress;
	}

	public void setProgress(float progress) {
		this.progress = progress;
		this.invalidate() ;
	}
}
