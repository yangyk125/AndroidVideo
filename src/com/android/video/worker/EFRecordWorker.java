package com.android.video.worker;

import com.android.nxsdk.NXWorker;

public class EFRecordWorker extends NXWorker {
	
	public interface EFRecordWorkerProgress {
		public void onRecordSegmentsDuration(float duration) ;
		public void onDeleteLastSegment(float duration) ;
		public void onDeleteAllTheSegments() ;
	}
	
	public interface EFBuildrWorkerProgress {
		public void onBuildProgressBegin();
		public void onBuildProgressProgress(float progress); //0.0-1.0
		public void onBuildProgressFailure();
		public void onBuildProgressComplete(String buildFile);
	}
		
	public final native boolean createRecordWorker(Object surface, int cameraWidth, int cameraHeight, String rootFolder);
	public final native boolean updateCameraImage(int width, int height, byte[] data);
	public final native boolean destoryRecordWorker();
	
	public final native boolean pausedRecordWorker();
	public final native boolean resumeRecordWorker();

	public final native boolean setRecordProgress(EFRecordWorkerProgress progress) ;
	
	public final native boolean pauseLoopAnimation() ;
	public final native boolean resumeLoopAnimation() ;
	
	//record mode api
	public final native boolean startRecordVideo();
	public final native boolean stopRecordVideo();
	public final native boolean deleteRecordVideo();
	
	public final native boolean showFilterPanel();
	public final native boolean hideFilterPanel(int index);
	
	public final native boolean buildRecordVideos(EFBuildrWorkerProgress progress);
	public final native boolean buildFinishVideos();
}
