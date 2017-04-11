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
import java.util.Enumeration;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

public class EFEffectsManager implements Runnable , Handler.Callback{
	
	public static interface EFEffectsListener {
		public void onEffectFileDownloadReady(String effectFile) ;
		public void onEffectFileDownloadStart(String effectFile) ;
		public void onEffectFileDownloadProgress(String effectFile, float progress) ;
		public void onEffectFileDownloadFailure(String effectFile) ;
		public void onEffectFileDownloadComplete(String effectFile, String cacheFolder) ;
	}
	

	public static class EFEffectDownMission {
		private String 				effectURL    = null ;
		private String 				effectTemp   = null ;
		private String				effectCache  = null ;
		private EFEffectsListener	httpListener = null ;
		
		public String getEffectURL() {
			return effectURL;
		}
		public void setEffectURL(String effectURL) {
			this.effectURL = effectURL;
		}
		public String getEffectTemp() {
			return effectTemp;
		}
		public void setEffectTemp(String effectTemp) {
			this.effectTemp = effectTemp;
		}
		public String getEffectCache() {
			return effectCache;
		}
		public void setEffectCache(String effectCache) {
			this.effectCache = effectCache;
		}
		public EFEffectsListener getHttpListener() {
			return httpListener;
		}
		public void setHttpListener(EFEffectsListener httpListener) {
			this.httpListener = httpListener;
		}
	}
	
	protected static final int	HANDLE_MESSAGE_EFFECT_READY = 2001 ;
	protected static final int	HANDLE_MESSAGE_EFFECT_START = 2002 ;
	protected static final int	HANDLE_MESSAGE_EFFECT_PROGS = 2003 ;
	protected static final int	HANDLE_MESSAGE_EFFECT_SCUSS = 2004 ;
	protected static final int	HANDLE_MESSAGE_EFFECT_FAILD = 2005 ;

	private static EFEffectsManager shareManager = null ;
	private static int				threadCount  = 3    ;
	private static String			sdcardFolder = "/sdcard" ;
	private static String			chacheFolder = "/e4fun/effects" ;
	private static String			downldFolder = String.format("%s%s", sdcardFolder, chacheFolder) ;
	
	private Thread[] 				  missionThread = null ;
	private List<EFEffectDownMission> missionList   = Collections.synchronizedList(new LinkedList<EFEffectDownMission>()) ; 
	
	protected Handler				  missionHandler = null ;
	
	public static EFEffectsManager getShareManager() {
		if(shareManager == null) 
			shareManager = new EFEffectsManager(threadCount);
		return shareManager ;
	}
	
	protected EFEffectsManager(int threads) {
		this.getEffectsFolder() ;
		this.missionThread  = new Thread[threads] ;
		this.missionHandler = new Handler(this) ;
		for(int idx=0; idx<threads; idx++) {
			this.missionThread[idx] = new Thread(this);
			this.missionThread[idx].start() ;
		}
	}
	
	public boolean handleMessage(Message msg) {
		EFEffectDownMission mission = (EFEffectDownMission)msg.obj ;
		switch(msg.what) {
		case HANDLE_MESSAGE_EFFECT_START:
			mission.getHttpListener().onEffectFileDownloadStart(mission.getEffectURL()) ;
			break ;
		case HANDLE_MESSAGE_EFFECT_PROGS:
			mission.getHttpListener().onEffectFileDownloadProgress(mission.getEffectURL(), 1.0f*msg.arg1/10000) ;
			break ;
		case HANDLE_MESSAGE_EFFECT_SCUSS:
			mission.getHttpListener().onEffectFileDownloadComplete(mission.getEffectURL(), mission.getEffectCache()) ;
			break ;
		case HANDLE_MESSAGE_EFFECT_FAILD:
			mission.getHttpListener().onEffectFileDownloadFailure(mission.getEffectURL()) ;
			break ;
		}
		return true ;
	}

	public String getEffectsFolder() {
		File folderObj = new File(downldFolder);
		if(!folderObj.exists())
			folderObj.mkdirs() ;
		return downldFolder ;
	}
	
	public boolean isEffectCached(Map<String, Object> effectInfo) {
		String effectZip = (String)effectInfo.get("config") ;
		return this.isEffectCached(effectZip) ;
	}
	
	public boolean isEffectCached(String effectFile) {
		String effMd5 = E4FunTool.MD5(effectFile) ;
		String effFolder = String.format("%s/%s", getEffectsFolder(), effMd5) ;
		return new File(effFolder).exists() ;
	}
	
	public String  getEffectFolder(Map<String, Object> effectInfo) {
		String effectZip = (String)effectInfo.get("config") ;
		return this.getEffectFolder(effectZip) ;
	}
	
	public String  getEffectFolder(String effectFile) {
		String effMd5 = E4FunTool.MD5(effectFile) ;
		String effFolder = String.format("%s/%s", getEffectsFolder(), effMd5) ;
		return effFolder ;
	}
	
	public void fetchEffectFile(Map<String, Object> effectInfo, EFEffectsListener listener) {
		String effectZip = (String)effectInfo.get("config") ;
		this.fetchEffectFile(effectZip, listener);
	}
	
	public void fetchEffectFile(String effectFile, EFEffectsListener listener) {
		EFEffectDownMission mission = new EFEffectDownMission();
		mission.setEffectURL(effectFile) ;
		String effectTemp = String.format("%s/%s_temp", getEffectsFolder(), E4FunTool.MD5(effectFile)) ;
		mission.setEffectTemp(effectTemp) ;
		String effectFolder = String.format("%s/%s", getEffectsFolder(), E4FunTool.MD5(effectFile)) ;
		mission.setEffectCache(effectFolder) ;
		mission.setHttpListener(listener) ;
		this.addDownMission(mission) ;
		this.missionHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_EFFECT_READY, mission)) ;
	}

	private void addDownMission(EFEffectDownMission mission) {
		missionList.add(mission) ;
	}
	
	public void run() {
		
		while(this.missionThread != null) {
			EFEffectDownMission currnetMission = null ;
			synchronized(missionList) {
				if(missionList.size() > 0) {
					currnetMission = missionList.get(0) ;
					missionList.remove(0) ;
				}
			}
			
			if(currnetMission != null) {
				
				if(currnetMission.getHttpListener() != null)
					this.missionHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_EFFECT_START, currnetMission)) ;
						
				if(this.isEffectCached(currnetMission.getEffectURL())) {
					if(currnetMission.getHttpListener() != null) 
						this.missionHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_EFFECT_SCUSS, currnetMission)) ;
					continue ;
				}

				try {

					HttpClient httpClient = new HttpClient() ;
					HttpMethodBase httpMethod = new GetMethod(currnetMission.getEffectURL()) ;

					int httpStatusCode = httpClient.executeMethod(httpMethod) ;

					if(httpStatusCode/100 != 2) {
						if(currnetMission.getHttpListener() != null)
							this.missionHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_EFFECT_FAILD, currnetMission)) ;
						httpMethod.releaseConnection() ;
						continue ;
					}
					
					byte[] buffer = new byte[1024] ;
					long length = Math.max(httpMethod.getResponseContentLength(), 1)  ;
					InputStream reader = httpMethod.getResponseBodyAsStream() ;
					FileOutputStream writer = new FileOutputStream(currnetMission.getEffectTemp());

					int readlen = 0 ;
					int currlen = 0 ;
					int readcnt = 0 ;
					while((currlen = reader.read(buffer, 0, buffer.length)) > 0) {
						writer.write(buffer, 0, currlen) ;
						readlen += currlen ;
						readcnt ++ ;
						if(currnetMission.getHttpListener() != null && readcnt%10 == 1)
							this.missionHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_EFFECT_PROGS, (int)(8000*1.0f*readlen/length), 0, currnetMission)) ;
					}
					
					writer.flush() ;
					writer.close() ;
					reader.close() ;
					
					httpMethod.releaseConnection() ;
					
					if(currnetMission.getHttpListener() != null) 
						this.missionHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_EFFECT_PROGS, 8000, 0, currnetMission)) ;
						
					//解压zip
					ZipFile zfile = new ZipFile(currnetMission.getEffectTemp()) ;
					Enumeration<?> zlist = zfile.entries() ;
					
					File folder = new File(currnetMission.getEffectCache()) ;
					folder.mkdirs() ;

					int fileCount = zfile.size() ;
					int fileIndex = 0 ;
					while(zlist.hasMoreElements()) {
						ZipEntry entry = (ZipEntry)zlist.nextElement() ;
						if(!entry.isDirectory()){
							String filepath = String.format("%s/%s", currnetMission.getEffectCache(), entry.getName()) ;
							FileOutputStream zipWriter = new FileOutputStream(filepath);
							InputStream zipReader = zfile.getInputStream(entry) ;

							int zcurlen = 0 ;
							while((zcurlen=zipReader.read(buffer, 0, buffer.length)) > 0)
								zipWriter.write(buffer, 0, zcurlen) ;
							
							zipWriter.flush() ;
							zipWriter.close() ;
							
							zipReader.close() ;
						}
						fileIndex ++ ;
						if(currnetMission.getHttpListener() != null) 
							this.missionHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_EFFECT_PROGS, 8000+2000*fileIndex/fileCount, 0, currnetMission)) ;
					}
					
					zfile.close() ;
					new File(currnetMission.getEffectTemp()).delete() ;
					
					if(currnetMission.getHttpListener() != null) 
						this.missionHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_EFFECT_SCUSS, currnetMission)) ;
				}
				catch(Exception e) {
					if(currnetMission.getHttpListener() != null)
						this.missionHandler.sendMessage(Message.obtain(null, HANDLE_MESSAGE_EFFECT_FAILD, currnetMission)) ;
					e.printStackTrace() ;
				}
			}
			else
				try{Thread.sleep(100) ;}catch(Exception e) {}
		}
	}
}
