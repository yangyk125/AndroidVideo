package com.android.video.views;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.AttributeSet;
import android.widget.ImageView;

import com.android.video.https.EFDownloadManager.EFDownloadListener;
import com.android.video.tools.EFBitmapManager;

public class EFImageView extends ImageView implements EFDownloadListener {

	
	public EFImageView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
	}

	public EFImageView(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	public EFImageView(Context context) {
		super(context);
	}

	public void onFileDownloadStart(String fileUrl) {

	}

	public void onFileDownloadProgress(String fileUrl, float progress) {

	}

	public void onFileDownloadFailure(String fileUrl) {
		this.setBackgroundColor(0xFFFF0000) ;
	}

	public void onFileDownloadComplete(String fileUrl, String cachePath) {
		Bitmap bitmap = EFBitmapManager.loadBitmapFromFile(cachePath) ;
		if(bitmap != null)
			this.setImageBitmap(bitmap) ;
	}

}
