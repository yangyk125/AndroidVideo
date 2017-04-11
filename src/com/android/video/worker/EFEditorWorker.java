package com.android.video.worker;

import com.android.nxsdk.NXWorker;

public class EFEditorWorker extends NXWorker {

	public interface EFPlayerWorkerProgress {
		public void onPlayerLoadedCompleted(float duration) ;
		public void onPlayerPlayingProgress(float progress) ;
		public void onPlayerPlayingStart() ;
		public void onPlayerPlayingPause() ;
		public void onPlayerPlayingStoped() ;
	}
	
	public interface EFEditorWorkerProgress {
		public void onDeleteEffectItem(int effectKey) ;
	}
	
	public interface EFRenderWorkerProgress {
		public void onRenderProgressBegin();
		public void onRenderProgressProgress(float progress); //0.0-1.0
		public void onRenderProgressFailure();
		public void onRenderProgressComplete(String buildFile);
	}
	
	public interface EFExtractWorkerProgress {
		public void onExtractImagesStart(int count, int width, int height) ;
		public void onExtractImagesIndex(int index, int width, int height, int[] pixels) ;
		public void onExtractImagesStop() ;
	}
	
	public final static int MAIN_WORKER_TOUCH_DOWN   = 1 ;
	public final static int MAIN_WORKER_TOUCH_MOVE   = 2 ;
	public final static int MAIN_WORKER_TOUCH_UP     = 3 ;
	
	public final native boolean createEditorWorker(Object surface, String rootFolder, String recordVideo);
	public final native boolean updateTouchEvent(int type, float x, float y) ;
	public final native boolean destoryEditorWorker();
	
	public final native boolean pausedEditorWorker() ;
	public final native boolean resumeEditorWorker() ;
	
	public final native boolean pauseLoopAnimation() ;
	public final native boolean resumeLoopAnimation() ;
	
	//edit mode api
	public final native boolean setPlayerProgress(EFPlayerWorkerProgress progress) ;
	public final native boolean setEditorProgress(EFEditorWorkerProgress progress) ;
	
	public final native boolean loadVideoPlayer() ;
	public final native boolean unloadVideoPlayer() ;

	public final native boolean startPlayVideo();
	public final native boolean pausePlayVideo();
	public final native boolean stopPlayVideo();
	public final native boolean restartPlayVideo();
	
	public final native boolean renderEffectVideo(EFRenderWorkerProgress progress);
	public final native boolean renderFinishVideo();
	
	//edit effect api
	public final native boolean syncEffectDatas(String effects) ; //all effect data structs json
	public final native boolean seekPlayPosition(float pos) ; //0.0-1.0f
	public final native boolean extractSerialImages(int count, int width, int height, EFExtractWorkerProgress progress) ;
	
	public final native boolean cleanCocos2dXCache() ;
}
