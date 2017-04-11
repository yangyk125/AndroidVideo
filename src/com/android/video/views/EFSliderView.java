package com.android.video.views;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.View;

public class EFSliderView extends View {

	private EFSliderView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
	}

	private EFSliderView(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	private EFSliderView(Context context) {
		super(context);
	}

	public EFSliderView(Context context, float max) {
		super(context);
		this.sliderMax = max ;
		this.sliderVal = 0.0f ;
	}
	
	private int backColor = 0xFF666666 ;
	private int stepColor = 0xFF22CC44 ;
	
	private float	sliderMax = 1.0f ;
	private float	sliderVal = 0.0f ;
	
	private Paint 	colorPaint = new Paint();
	private Rect	colorRect  = new Rect();
	
	public int getBackColor() {
		return backColor;
	}

	public void setBackColor(int backColor) {
		this.backColor = backColor;
	}

	public int getStepColor() {
		return stepColor;
	}

	public void setStepColor(int stepColor) {
		this.stepColor = stepColor;
	}
	
	public float getSliderMax() {
		return sliderMax;
	}

	public void setSliderMax(float sliderMax) {
		this.sliderMax = sliderMax;
	}

	public float getSliderValue() {
		return sliderVal;
	}

	public void setSliderValue(float sliderValue) {
		this.sliderVal = sliderValue;
		this.invalidate(); 
	}

	public void draw(Canvas canvas) {
		super.draw(canvas);
		
		this.colorPaint.setColor(this.backColor);
		this.getDrawingRect(this.colorRect);
		canvas.drawRect(this.colorRect, this.colorPaint);
		
		this.colorPaint.setColor(this.stepColor);
		this.colorRect.right = (int)(sliderVal/sliderMax * this.colorRect.width()) ;
		canvas.drawRect(this.colorRect, this.colorPaint);
	}
	
}
