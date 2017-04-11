package com.android.video.https;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.HttpMethodBase;
import org.apache.commons.httpclient.NameValuePair;
import org.apache.commons.httpclient.methods.GetMethod;
import org.apache.commons.httpclient.methods.PostMethod;

import android.os.Handler;
import android.os.Message;

public class EFRequestsManager implements Runnable, Handler.Callback {

	public static class EFRequest {
		
		public static final int 		GET 	 	= 1 ;
		public static final int 		POST	 	= 2 ;
		
		protected String 				URL 	 	= null  ;
		protected int 					method 		= GET ;
		protected Map<String, Object> 	fields 		= new HashMap<String, Object>() ; 
		protected EFRequestsListener	listener    = null ;
		
		protected int					status		= 0 ; //http code .
		protected String				content		= null ;
		
		//业务数据
		protected int 					context1    = 0 ;
		protected long 					context2    = 0 ;
		protected Object 				context3    = null ;
		protected Object 				context4    = null ;
		
		
		public String getURL() {
			return URL;
		}
		public void setURL(String uRL) {
			URL = uRL;
		}
		public int getMethod() {
			return method;
		}
		public void setMethod(int method) {
			this.method = method;
		}
		public Map<String, Object> getFields() {
			return fields;
		}
		public EFRequestsListener getListener() {
			return listener;
		}
		public void setListener(EFRequestsListener listener) {
			this.listener = listener;
		}
		public int getStatus() {
			return status;
		}
		public void setStatus(int status) {
			this.status = status;
		}
		public String getContent() {
			return content;
		}
		public void setContent(String content) {
			this.content = content;
		}
		public int getContext1() {
			return context1;
		}
		public void setContext1(int context1) {
			this.context1 = context1;
		}
		public long getContext2() {
			return context2;
		}
		public void setContext2(long context2) {
			this.context2 = context2;
		}
		public Object getContext3() {
			return context3;
		}
		public void setContext3(Object context3) {
			this.context3 = context3;
		}
		public Object getContext4() {
			return context4;
		}
		public void setContext4(Object context4) {
			this.context4 = context4;
		}
	}
	
	public static interface EFRequestsListener {
		public void onRequestStart(EFRequest request) ;
		public void onRequestFailure(EFRequest request) ;
		public void onRequestComplete(EFRequest request) ;
	}
	
	public static EFRequest buildRequest(String url, int method, String... fields) {
		EFRequest request = new EFRequest() ;
		request.setURL(url) ;
		request.setMethod(method) ;
		if(fields != null) {
			for(int idx=0; idx<fields.length/2; idx++) {
				request.getFields().put(fields[idx], fields[idx+1]) ;
			}
		}
		return request ;
	}
	
	
	protected static final String		 httpServer   = "http://test.test.com" ;
	
	protected static EFRequestsManager 	 shareManager = null ;
	protected static int				 threadCount  = 5    ;
	
	protected static final int			 HANDLE_MESSAGE_REQUEST_START  = 100 ;
	protected static final int			 HANDLE_MESSAGE_REQUEST_SUCCSS = 200 ;
	protected static final int			 HANDLE_MESSAGE_REQUEST_FAILED = 300 ;
	
	protected List<EFRequest> 	requestsQueue  = Collections.synchronizedList(new LinkedList<EFRequest>()) ;
	protected Thread[] 			requestThreads = null ;
	protected Handler			requestHandler = null ;
	
	public static EFRequestsManager getShareManager() {
		if(shareManager == null) 
			shareManager = new EFRequestsManager(threadCount);
		return shareManager ;
	}
	
	public static String getHttpServer() {
		return httpServer ;
	}
	
	protected EFRequestsManager(int threads) {
		this.requestHandler = new Handler(this) ;
		this.requestThreads = new Thread[threads] ;
		for(int idx=0; idx<threads; idx++) {
			this.requestThreads[idx] = new Thread(this);
			this.requestThreads[idx].start() ;
		}
	}
	
	public void addRequest(EFRequest request) {
		requestsQueue.add(request) ;
	}

	public boolean handleMessage(Message msg) {
		EFRequest request = (EFRequest)msg.obj ;
		if(request.getListener() != null) {
			switch(msg.what) {
			case HANDLE_MESSAGE_REQUEST_START:
				request.getListener().onRequestStart(request) ;
				break ;
			case HANDLE_MESSAGE_REQUEST_SUCCSS:
				request.getListener().onRequestComplete(request) ;
				break ;
			case HANDLE_MESSAGE_REQUEST_FAILED:
				request.getListener().onRequestFailure(request) ;
				break ;
			}
		}
		return true;
	}

	public void run() {
		
		while(this.requestThreads != null) {
			
			EFRequest request = null ;
			synchronized(requestsQueue) {
				if(requestsQueue.size() > 0) {
					request = requestsQueue.get(0) ;
					requestsQueue.remove(0) ;
				}
			}
			
			if(request != null) {
				
				try {
					
					if(request.getListener() != null) 
						this.requestHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_REQUEST_START, request)) ;
					
					HttpClient httpClient = new HttpClient() ;
					HttpMethodBase httpMethod = null ;
					
					if(request.method == EFRequest.GET)
						httpMethod = new GetMethod(request.getURL()) ;
					else
						httpMethod = new PostMethod(request.getURL()) ;
					
					List<NameValuePair> params = new ArrayList<NameValuePair>() ;
					
					Set<String> keys = request.getFields().keySet() ;
					for(String key : keys) {
						Object value = request.getFields().get(key) ;
						if(value instanceof String)
							params.add(new NameValuePair(key, (String)value)) ;
					}
					
					if(params.size() > 0)
						httpMethod.setQueryString((NameValuePair[])params.toArray()) ;
					
					int statusCode = httpClient.executeMethod(httpMethod) ;
					
					if(statusCode/100 != 2) {
						if(request.getListener() != null)
							this.requestHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_REQUEST_FAILED, request)) ;
						httpMethod.releaseConnection() ;
						continue ;
					}
					
					String httpContent = httpMethod.getResponseBodyAsString() ;
					httpMethod.releaseConnection() ;
					
					if(statusCode/100 != 2) {
						if(request.getListener() != null)
							this.requestHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_REQUEST_FAILED, request)) ;
						continue ;
					}

					request.setStatus(statusCode) ;
					request.setContent(httpContent) ;

					if(request.getListener() != null)
						this.requestHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_REQUEST_SUCCSS, request)) ;
				}
				catch(Exception e) {
					if(request.getListener() != null)
						this.requestHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_REQUEST_FAILED, request)) ;
				}
			}
			else
				try{Thread.sleep(100) ;}catch(Exception e) {}
		}
	}

}
