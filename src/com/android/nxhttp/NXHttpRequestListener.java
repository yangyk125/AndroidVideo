package com.android.nxhttp;

public interface NXHttpRequestListener {
	/*
	 * http request work begin
	 */
	public void OnNXHttpRequestStart(NXHttpRequest request) ;
	
	/*
	 * http request begin send data
	 */
	public void OnNXHttpRequestSendBegin(NXHttpRequest request) ;
	
	/*
	 * http request sending progress
	 */
	public void OnNXHttpRequestSendProgress(NXHttpRequest request, float progress) ; //0.0-1.0
	
	/*
	 * http request sending complete
	 */
	public void OnNXHttpRequestSendComplete(NXHttpRequest request) ;
	
	/*
	 * http request recv begin
	 */
	public void OnNXHttpRequestRecvBegin(NXHttpRequest request) ;
	
	/*
	 * http request recv progress
	 */
	public void OnNXHttpRequestRecvProgress(NXHttpRequest request, float progress) ;//0.0-1.0F
	
	/*
	 * http request recv complete
	 */
	public void OnNXHttpRequestRecvComplete(NXHttpRequest request) ;
	
	/*
	 * http request work success
	 */
	public void OnNXHttpRequestSuccess(NXHttpRequest request) ;
	
	/*
	 * http request work failure
	 */
	public void OnNXHttpRequestFailure(NXHttpRequest request) ;
}
