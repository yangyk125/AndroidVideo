package com.android.video.tools;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;

import org.xml.sax.helpers.DefaultHandler;

import android.xml.plist.PListXMLHandler;
import android.xml.plist.PListXMLParser;

public class EFPlistParser extends DefaultHandler{
	
	public static Object parsePlistWithFile(File plistFile) {
		try {
			InputStream instream = new FileInputStream(plistFile) ;
			Object result = parsePlistWithStream(instream) ;
			instream.close(); 
			return result ;
		}
		catch(Exception e) {
			e.printStackTrace();
		}
		return null ;
	}
	
	public static Object parsePlistWithStream(InputStream instream) {
		try {
			PListXMLParser parser = new PListXMLParser();
			PListXMLHandler handler = new PListXMLHandler();
			parser.setHandler(handler);
			parser.parse(instream) ;
			return handler.getPlist().getRootElement() ;
		}
		catch(Exception e) {
			e.printStackTrace(); 
		}
		return null ;
	}
	
	public static Object parsePlistWithString(String plistData) {
		try {
			PListXMLParser parser = new PListXMLParser();
			PListXMLHandler handler = new PListXMLHandler();
			parser.setHandler(handler);
			parser.parse(plistData) ;
			return handler.getPlist().getRootElement() ;
		}
		catch(Exception e) {
			e.printStackTrace(); 
		}
		return null ;
	}
}
