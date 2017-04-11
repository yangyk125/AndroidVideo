package com.android.video.https;

import android.os.Handler;
import android.os.Message;

import com.android.video.tools.E4FunTool;

import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.HttpMethodBase;
import org.apache.commons.httpclient.methods.GetMethod;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;

public class EFDownloadManager implements Runnable, Handler.Callback{
	
	public static interface EFDownloadListener {
		public void onFileDownloadStart(String fileUrl) ;
		public void onFileDownloadProgress(String fileUrl, float progress) ;
		public void onFileDownloadFailure(String fileUrl) ;
		public void onFileDownloadComplete(String fileUrl, String cachePath) ;
	}
	
	public static class EFDownloadMission {
		private String 				httpFileURL    = null ;
		private String 				httpFileTemp   = null ;
		private String				httpFileCache  = null ;
		private EFDownloadListener	httpFileListener = null ;
		public String getHttpFileURL() {
			return httpFileURL;
		}
		public void setHttpFileURL(String httpFileURL) {
			this.httpFileURL = httpFileURL;
		}
		public String getHttpFileTemp() {
			return httpFileTemp;
		}
		public void setHttpFileTemp(String httpFileTemp) {
			this.httpFileTemp = httpFileTemp;
		}
		public String getHttpFileCache() {
			return httpFileCache;
		}
		public void setHttpFileCache(String httpFileCache) {
			this.httpFileCache = httpFileCache;
		}
		public EFDownloadListener getHttpFileListener() {
			return httpFileListener;
		}
		public void setHttpFileListener(EFDownloadListener httpFileListener) {
			this.httpFileListener = httpFileListener;
		}
	}
	
	protected static final int	HANDLE_MESSAGE_MISSION_START = 1000 ;
	protected static final int	HANDLE_MESSAGE_MISSION_PROGS = 2000 ;
	protected static final int	HANDLE_MESSAGE_MISSION_SCUSS = 3000 ;
	protected static final int	HANDLE_MESSAGE_MISSION_FAILD = 4000 ;

	private static EFDownloadManager shareManager = null ;
	private static int				 threadCount  = 1    ;
	private static String			 sdcardFolder = "/sdcard" ;
	private static String			 chacheFolder = "/e4fun/caches" ;
	private static String			 downldFolder = String.format("%s%s", sdcardFolder, chacheFolder) ;
	
	private Thread[] 				  missionThread = null ;
	private List<EFDownloadMission>   missionList   = Collections.synchronizedList(new LinkedList<EFDownloadMission>()) ; 
	
	protected Handler				  missionHandler = null ;
	public static EFDownloadManager getShareManager() {
		if(shareManager == null) 
			shareManager = new EFDownloadManager(threadCount);
		return shareManager ;
	}
	
	protected EFDownloadManager(int threads) {
		this.getCacheFileFolder() ;
		this.missionThread = new Thread[threads] ;
		this.missionHandler = new Handler(this) ;
		for(int idx=0; idx<threads; idx++) {
			this.missionThread[idx] = new Thread(this);
			this.missionThread[idx].start() ;
		}
	}
	
	public boolean handleMessage(Message msg) {
		EFDownloadMission mission = (EFDownloadMission)msg.obj ;
		switch(msg.what) {
		case HANDLE_MESSAGE_MISSION_START:
			mission.getHttpFileListener().onFileDownloadStart(mission.getHttpFileURL()) ;
			break ;
		case HANDLE_MESSAGE_MISSION_PROGS:
			mission.getHttpFileListener().onFileDownloadProgress(mission.getHttpFileURL(), 1.0f*msg.arg1/10000) ;
			break ;
		case HANDLE_MESSAGE_MISSION_SCUSS:
			mission.getHttpFileListener().onFileDownloadComplete(mission.getHttpFileURL(), mission.getHttpFileCache()) ;
			break ;
		case HANDLE_MESSAGE_MISSION_FAILD:
			mission.getHttpFileListener().onFileDownloadFailure(mission.getHttpFileURL()) ;
			break ;
		}
		return true ;
	}

	public String getCacheFileFolder() {
		File folderObj = new File(downldFolder);
		if(!folderObj.exists())
			folderObj.mkdirs() ;
		return downldFolder ;
	}
	
	public String buildTempPath(String fileUrl) {
		String suffix = "" ;	
		int pointIndex = fileUrl.lastIndexOf('.') ;
		if(pointIndex >= 0)
			suffix = fileUrl.substring(pointIndex) ;
		String effectTemp = String.format("%s/%s%s.tmp", getCacheFileFolder(), E4FunTool.MD5(fileUrl), suffix) ;
		return effectTemp ;
	}
	
	public String buildCachePath(String fileUrl) {
		String suffix = "" ;	
		int pointIndex = fileUrl.lastIndexOf('.') ;
		if(pointIndex >= 0)
			suffix = fileUrl.substring(pointIndex) ;
		String effectCache = String.format("%s/%s%s", getCacheFileFolder(), E4FunTool.MD5(fileUrl), suffix) ;
		return effectCache ;
	}

	public boolean isHttpFileCached(String fileUrl) {
		String cachePath = buildCachePath(fileUrl) ;
		return new File(cachePath).exists() ;
	}

	public String getHttpFilePath(String fileUrl) {
		String cachePath = buildCachePath(fileUrl) ;
		File cacheFile = new File(cachePath);
		if(cacheFile.exists())
			return cachePath ;
		return null ;
	}

	public void fetchHttpFile(String fileUrl, EFDownloadListener listener) {
		
		String httpCacheFile = getHttpFilePath(fileUrl);
		if(httpCacheFile != null) {
			if(listener != null) 
				listener.onFileDownloadComplete(fileUrl, httpCacheFile) ;
			return ;
		}
		
		EFDownloadMission mission = new EFDownloadMission();
		mission.setHttpFileURL(fileUrl) ;
		String fileTemp = buildTempPath(fileUrl);
		mission.setHttpFileTemp(fileTemp) ;
		String fileCache = buildCachePath(fileUrl);
		mission.setHttpFileCache(fileCache) ;
		mission.setHttpFileListener(listener) ;
		
		this.addDownMission(mission) ;
	}

	private void addDownMission(EFDownloadMission mission) {
		missionList.add(mission) ;
	}
	
	public void run() {
		
		while(this.missionThread != null) {
			
			EFDownloadMission currnetMission = null ;
			synchronized(missionList) {
				if(missionList.size() > 0) {
					currnetMission = missionList.get(0) ;
					missionList.remove(0) ;
				}
			}
			
			if(currnetMission != null) {
				
				if(currnetMission.getHttpFileListener() != null)
					this.missionHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_MISSION_START, currnetMission)) ;
				
				String httpCacheFile = getHttpFilePath(currnetMission.getHttpFileURL());
				if(httpCacheFile != null) {
					if(currnetMission.getHttpFileListener() != null) 
						this.missionHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_MISSION_SCUSS, currnetMission)) ;
					continue ;
				}
							
				try {

					HttpClient httpClient = new HttpClient() ;
					HttpMethodBase httpMethod = new GetMethod(currnetMission.getHttpFileURL()) ;

					int httpStatusCode = httpClient.executeMethod(httpMethod) ;

					if(httpStatusCode/100 != 2) {
						if(currnetMission.getHttpFileListener() != null)
							this.missionHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_MISSION_FAILD, currnetMission)) ;
						httpMethod.releaseConnection() ;
						continue ;
					}
					
					byte[] buffer = new byte[1024] ;
					long length = Math.max(httpMethod.getResponseContentLength(), 1)  ;
					File tempFile = new File(currnetMission.getHttpFileTemp());
					InputStream reader = httpMethod.getResponseBodyAsStream() ;
					FileOutputStream writer = new FileOutputStream(tempFile);

					int readlen = 0 ;
					int currlen = 0 ;
					int readcnt = 0 ;
					while((currlen = reader.read(buffer, 0, buffer.length)) > 0) {
						writer.write(buffer, 0, currlen) ;
						readlen += currlen ;
						readcnt ++ ;
						if(currnetMission.getHttpFileListener() != null && readcnt%10 == 1)
							this.missionHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_MISSION_PROGS, (int)(10000*1.0f*readlen/length), 0, currnetMission)) ;
					}
					
					writer.flush() ;
					writer.close() ;
					reader.close() ;
					
					httpMethod.releaseConnection() ;
					
					if(currnetMission.getHttpFileListener() != null) 
						this.missionHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_MISSION_PROGS, 10000, 0, currnetMission)) ;
					
					if(tempFile.renameTo(new File(currnetMission.getHttpFileCache()))) {
						if(currnetMission.getHttpFileListener() != null) 
							this.missionHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_MISSION_SCUSS, currnetMission)) ;
					}
					else {
						if(currnetMission.getHttpFileListener() != null) 
							this.missionHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_MISSION_FAILD, currnetMission)) ;
					}
				}
				catch(Exception e) {
					if(currnetMission.getHttpFileListener() != null)
						this.missionHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_MISSION_FAILD, currnetMission)) ;
					e.printStackTrace() ;
				}
			}
			else
				try{Thread.sleep(100) ;}catch(Exception e) {}
		}
	}
}
