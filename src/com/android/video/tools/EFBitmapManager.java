package com.android.video.tools;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

public class EFBitmapManager {
	
	private static Map<String, Bitmap>  cacheBitmapMap  = new HashMap<String, Bitmap>();
	private static List<String>			cacheBitmapPath = new ArrayList<String>() ;
	
	public static Bitmap loadBitmapFromFile(String file) {
		
		Bitmap bitmap = cacheBitmapMap.get(file) ;
		if(bitmap == null) {
			bitmap = BitmapFactory.decodeFile(file) ;
			cacheBitmapMap.put(file, bitmap) ;
			cacheBitmapPath.add(file) ;
		}
		
		if(cacheBitmapPath.size() > 40) {
			for(int idx=0; idx<20; idx++) {
				String path = cacheBitmapPath.remove(0) ;
				cacheBitmapMap.remove(path) ;
			}
		}
		
		return bitmap ;
	}
}
