package com.android.nxsdk;

public class NXWorker {
	
	static {
		System.loadLibrary("WORKER");
	}
	
	private static long workerRefs = 1000 ;
	private long  workerRef = 0 ;
	
	public NXWorker(){
		workerRef = ++workerRefs ;
	}
	
	public long getWorkerID() {
		return workerRef ;
	}
}
