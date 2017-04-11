package com.android.video.camera;

import java.util.List;

import android.content.Context;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.hardware.Camera.Size;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public final class EFCameraView extends SurfaceView implements SurfaceHolder.Callback, Camera.PreviewCallback{

	private int 						defaultWidth  = 640 ;
	private int 						defaultHeight = 480 ;
	
	private EFCameraViewListener		cameraListenrer = null ;
	private Camera 						cameraObject = null ;
	private int							previewFormat = -1 ;
	private int 						priviewWidth  = 0 ;
	private int 						priviewHeight = 0 ;
	private byte[] 						priviewBuffer = null ;
	private boolean 					previewDoing  = false ;
	
	public EFCameraView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		initCameraView();
	}

	public EFCameraView(Context context, AttributeSet attrs) {
		super(context, attrs);
		initCameraView();
	}

	public EFCameraView(Context context) {
		super(context);
		initCameraView();
	}
	
	private void initCameraView() {
		this.getHolder().addCallback(this);
	}
	
	protected void prepareCamera() {
		try {
			this.cameraObject = Camera.open(Camera.CameraInfo.CAMERA_FACING_BACK);
		} catch (Exception e) {
			e.printStackTrace() ;
		}
		
		if(cameraObject == null)
			return ;
		
		Camera.Parameters params = cameraObject.getParameters();
		params.setPreviewFormat(ImageFormat.NV21) ;
		this.previewFormat = params.getPreviewFormat();

		List<Size> preSizelist = params.getSupportedPictureSizes();
		boolean supportDefault = false;
		for (Size size : preSizelist) {
			if (size.width == defaultWidth && size.height == defaultHeight) {
				supportDefault = true;
				break;
			}
		}

		if (supportDefault) 
			params.setPreviewSize(defaultWidth, defaultHeight);

		this.cameraObject.setParameters(params);
		this.cameraObject.setDisplayOrientation(90);

		this.priviewWidth = params.getPreviewSize().width;
		this.priviewHeight = params.getPreviewSize().height;

		this.priviewBuffer = new byte[(int)(this.priviewWidth * this.priviewHeight * 1.5)];

		this.cameraObject.addCallbackBuffer(this.priviewBuffer);
		this.cameraObject.setPreviewCallbackWithBuffer(this);

		try {
			this.cameraObject.setPreviewDisplay(this.getHolder());
			this.cameraObject.startPreview();
			this.previewDoing = true ;
		} catch (Exception e) {
			e.printStackTrace() ;
		}
	}
	
	public void pausePreview(boolean pause) {
		if(pause)  {
			if(this.previewDoing && this.cameraObject != null) {
				this.cameraObject.stopPreview();
				this.previewDoing = false ;
			}
		}
		else {
			if(!this.previewDoing && this.cameraObject != null) {
				this.cameraObject.startPreview();
				this.previewDoing = true ;
			}
		}
	}

	public void onPreviewFrame(byte[] data, Camera camera) {
		camera.addCallbackBuffer(this.priviewBuffer);
		if(this.cameraListenrer != null)
			this.cameraListenrer.onCameraPreviewFrame(this.priviewWidth, this.priviewHeight, this.priviewBuffer, this.previewFormat) ;
	}

	public void surfaceCreated(SurfaceHolder holder) {
		 this.prepareCamera() ;
	}

	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

	}

	public void surfaceDestroyed(SurfaceHolder holder) {
		if(this.previewDoing)
			this.cameraObject.stopPreview();
		this.cameraObject.release();
	}

	public EFCameraViewListener getCameraListenrer() {
		return cameraListenrer;
	}

	public void setCameraListenrer(EFCameraViewListener cameraListenrer) {
		this.cameraListenrer = cameraListenrer;
	}

	public static interface EFCameraViewListener {
		public void onCameraPreviewFrame(int width, int height, byte[]data, int format);
	}

	public int getDefaultWidth() {
		return defaultWidth;
	}

	public void setDefaultWidth(int defaultWidth) {
		this.defaultWidth = defaultWidth;
	}

	public int getDefaultHeight() {
		return defaultHeight;
	}

	public void setDefaultHeight(int defaultHeight) {
		this.defaultHeight = defaultHeight;
	}
}
