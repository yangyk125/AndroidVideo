package com.android.nxhttp;

import java.util.LinkedList;
import java.util.List;

//import org.apache.http.client.HttpClient;
//import org.apache.http.impl.client.DefaultHttpClient;

public class NXHttpQueue implements Runnable {

	private NXHttpQueueListener listener = null ;
	
	private List<NXHttpRequest> requests = java.util.Collections.synchronizedList(new LinkedList<NXHttpRequest>()) ;
	
	public void run() {
		
	}

	public int addRequest(NXHttpRequest request) {
		requests.add(request) ;
		return requests.size() ;
	}
	
	public boolean cancelRequest(NXHttpRequest request) {
		return false ;
	}

	public NXHttpQueueListener getListener() {
		return listener;
	}

	public void setListener(NXHttpQueueListener listener) {
		this.listener = listener;
	}
}
