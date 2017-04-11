package com.android.video.tools;

import java.io.BufferedReader;
import java.io.CharArrayWriter;
import java.io.File;
import java.io.FileFilter;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.security.MessageDigest;
import java.util.regex.Pattern;

import android.util.Log;

public class E4FunTool {
	
	public static String getCPUName() {
		try {
			FileReader fr = new FileReader("/proc/cpuinfo");
			BufferedReader br = new BufferedReader(fr);
			String text = br.readLine();
			String[] array = text.split(":\\s+", 2);
			for (int i = 0; i < array.length; i++) {
				Log.d("APP", array[i]) ;
			}
			br.close(); 
			return array[1];
		} catch (IOException e) {
			e.printStackTrace();
		}
		return null;
	}

	public static int getCPUCores() {

		class CpuFilter implements FileFilter {
			public boolean accept(File pathname) {

				if (Pattern.matches("cpu[0-9]", pathname.getName())) {
					return true;
				}
				return false;
			}
		}

		try {
			File dir = new File("/sys/devices/system/cpu/");
			File[] files = dir.listFiles(new CpuFilter());
			return files.length;
		} catch (Exception e) {
			e.printStackTrace();
			return 1;
		}
	}

	public static String getCPUFreq() {
		String result = "";
		try {
			String[] args = { "/system/bin/cat", "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq" };
			ProcessBuilder cmd = new ProcessBuilder(args);
			Process process = cmd.start();
			InputStream in = process.getInputStream();
			byte[] re = new byte[24];
			while (in.read(re) != -1) {
				result = result + new String(re);
			}
			in.close();
		} catch (IOException ex) {
			ex.printStackTrace();
			result = "N/A";
		}
		return result.trim();
	}

	public static long getMemoryBytes() {
		String str1 = "/proc/meminfo";
		String str2;
		String[] arrayOfString;
		long initial_memory = 0;
		try {
			FileReader localFileReader = new FileReader(str1);
			BufferedReader localBufferedReader = new BufferedReader(localFileReader, 8192);
			str2 = localBufferedReader.readLine();

			arrayOfString = str2.split("\\s+");
			initial_memory = Integer.valueOf(arrayOfString[1]).intValue() * 1024;
			localBufferedReader.close();

		} catch (IOException e) {
		}

		return initial_memory;
	}
	
	public static String getSystemInfo(String procFile) {
		try {
			FileReader localFileReader = new FileReader(procFile);
			BufferedReader localBufferedReader = new BufferedReader(localFileReader, 8192);
			String content = localBufferedReader.readLine();
			localFileReader.close() ;
			localBufferedReader.close() ;
			return content ;

		} catch (IOException e) {
			return null ;
		}
	}
	
	public static void printPhotoInfos() {
		Log.d("APP", String.format("CPU Name = %s", getCPUName())) ;
		Log.d("APP", String.format("CPU Cout = %d", getCPUCores())) ;
		Log.d("APP", String.format("CPU Freq = %s", getCPUFreq())) ;
		Log.d("APP", String.format("Mem Byte = %d", (int)getMemoryBytes())) ;
	}
	
	public static String MD5(String source) {
		try {
			byte[] bytes = source.getBytes("utf8") ;
			MessageDigest mdInst = MessageDigest.getInstance("MD5") ;
			mdInst.update(bytes) ;
			byte[] digs = mdInst.digest() ;
			StringBuffer result = new StringBuffer();
			for(int idx=0; idx<digs.length; idx++) {
				byte byte0 = digs[idx] ;
				if(byte0 > 0)
					result.append(String.format("%02X", (byte)byte0)) ;
				else
					result.append(String.format("%02X", (byte)(255+byte0+1))) ;
			}
			return result.toString() ;
		}
		catch(Exception e) {
			e.printStackTrace();
		}
		
		return null ;
	}
	
	public static Object parseJson(String strJson) {
		EFJsonParser tokener = new EFJsonParser(strJson) ;
		try {
			return tokener.nextValue() ;
		}
		catch(Exception e) {}
		return null ;
	}
	
	public static String buildJson(Object jsonObj) {
		CharArrayWriter buffer = new CharArrayWriter();
		EFJsonBuilder jsonWriter = new EFJsonBuilder(buffer);
		jsonWriter.buildObject(jsonObj) ;
		return buffer.toString() ;
	}
}
