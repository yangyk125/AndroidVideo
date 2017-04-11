package com.android.video.page.widget;

import java.util.ArrayList;
import java.util.List;

import com.android.video.R;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.AttributeSet;
import android.widget.AbsoluteLayout;
import android.widget.ImageView;
import android.widget.ImageView.ScaleType;

@SuppressWarnings("deprecation")
public class EFEffectFilmView extends AbsoluteLayout {

	private ImageView		iconLeft = null ;
	private ImageView		iconRight = null ;
	
	private List<ImageView> filmImages = new ArrayList<ImageView>();
	
	public EFEffectFilmView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		initEffectFilmView() ;
	}

	public EFEffectFilmView(Context context, AttributeSet attrs) {
		super(context, attrs);
		initEffectFilmView() ;
	}

	public EFEffectFilmView(Context context) {
		super(context);
		initEffectFilmView() ;
	}

	protected void initEffectFilmView() {
		this.setPadding(0, 0, 0, 0) ;
		this.iconLeft = new ImageView(getContext()) ;
		this.iconRight = new ImageView(getContext()) ;
		this.iconLeft.setPadding(0, 0, 0, 0) ;
		this.iconRight.setPadding(0, 0, 0, 0) ;
		this.iconLeft.setScaleType(ScaleType.CENTER_CROP) ;
		this.iconRight.setScaleType(ScaleType.CENTER_CROP) ;
		this.iconLeft.setImageResource(R.drawable.icon_film_left) ;
		this.iconRight.setImageResource(R.drawable.icon_film_right) ;
		this.iconLeft.setLayoutParams(new AbsoluteLayout.LayoutParams(0, 0, 0, 0)) ;
		this.iconRight.setLayoutParams(new AbsoluteLayout.LayoutParams(0, 0, 0, 0)) ;
		this.addView(this.iconLeft) ;
		this.addView(this.iconRight) ;
	}

	public void assignFilmImages(int count, int width, int height) {
		
		for(ImageView imageView : this.filmImages) 
			this.removeView(imageView) ;
		this.filmImages.clear() ;
		
		for(int idx=0; idx<count; idx++) {
			ImageView imageView = new ImageView(getContext()) ;
			imageView.setPadding(0, 0, 0, 0) ;
			imageView.setScaleType(ScaleType.CENTER_CROP) ;
			imageView.setLayoutParams(new AbsoluteLayout.LayoutParams(0, 0, 0, 0)) ;
			this.addView(imageView) ;
			this.filmImages.add(imageView) ;
		}
		
		if(filmImages.size() > 0) {

			int subCount = filmImages.size()+1 ;
			int itemWidth = width /subCount ;
			
			int left = 0 ;
			AbsoluteLayout.LayoutParams layoutParam = (AbsoluteLayout.LayoutParams)this.iconLeft.getLayoutParams() ;
			layoutParam.x = left ;
			layoutParam.y = 0 ;
			layoutParam.width = itemWidth/2 ;
			layoutParam.height = height ;
			left += layoutParam.width ;
			
			for(int idx=0; idx<filmImages.size(); idx++) {
				ImageView subView = filmImages.get(idx) ;
				layoutParam = (AbsoluteLayout.LayoutParams)subView.getLayoutParams() ;
				layoutParam.x = left ;
				layoutParam.y = 0 ;
				layoutParam.width = itemWidth ;
				layoutParam.height = height ;
				left += layoutParam.width ;
			}
			
			layoutParam = (AbsoluteLayout.LayoutParams)this.iconRight.getLayoutParams() ;
			layoutParam.x = left ;
			layoutParam.y = 0 ;
			layoutParam.width = itemWidth/2 ;
			layoutParam.height = height ;
			left += layoutParam.height ;
		}
		
		this.requestLayout() ;
	}
	
	public void setFilmImage(int index, Bitmap bitmap) {
		this.filmImages.get(index).setImageBitmap(bitmap) ;
	}
	
	public void delFilmImages() {
		for(ImageView imageView : this.filmImages) 
			imageView.setImageBitmap(null);
	}
}
