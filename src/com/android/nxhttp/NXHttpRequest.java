package com.android.nxhttp;

import java.util.HashMap;
import java.util.Map;

public class NXHttpRequest {
	
	private NXHttpRequestListener		listener    = null ;
	
	/*Before Request Connected*/
	private String						URL     	= null ;
	private String  					method  	= null ;
	private Map<String, String>			headers 	= new HashMap<String, String>() ;
	private long						timeout 	= 10   ;
	private boolean 					saveAsFile 	= false ;
	
	/*After Request Connected*/
	private String						responseString  = null ;
	private String						responseFile    = null ;
	private int 						responseCode 	= 0 ;
	
	/*Other Properties*/
	private long						contentLength   = 0 ;
	

	public String getURL() {
		return URL;
	}

	public void setURL(String uRL) {
		URL = uRL;
	}

	public String getMethod() {
		return method;
	}

	public void setMethod(String method) {
		this.method = method;
	}

	public void addHttpHeader(String key, String value) {
		this.headers.put(key, value) ;
	}

	public void delHttpHeaders(String key) {
		this.headers.remove(key) ;
	}

	public long getTimeout() {
		return timeout;
	}

	public void setTimeout(long timeout) {
		this.timeout = timeout;
	}

	public boolean isSaveAsFile() {
		return saveAsFile;
	}

	public void setSaveAsFile(boolean saveAsFile) {
		this.saveAsFile = saveAsFile;
	}

	public String getResponseString() {
		return responseString;
	}

	public void setResponseString(String responseString) {
		this.responseString = responseString;
	}

	public String getResponseFile() {
		return responseFile;
	}

	public void setResponseFile(String responseFile) {
		this.responseFile = responseFile;
	}

	public int getResponseCode() {
		return responseCode;
	}

	public void setResponseCode(int responseCode) {
		this.responseCode = responseCode;
	}

	public long getContentLength() {
		return contentLength;
	}

	public void setContentLength(long contentLength) {
		this.contentLength = contentLength;
	}

	public NXHttpRequestListener getListener() {
		return listener;
	}

	public void setListener(NXHttpRequestListener listener) {
		this.listener = listener;
	}
}
