package com.android.video.tools;

import java.io.Writer;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class EFJsonBuilder {

	private Writer 		writer 		= null ;
	
	public EFJsonBuilder(Writer writer) {
		this.writer = writer ;
	}
	
	public void buildObject(Object value) {
		if(value instanceof List) 
			this.buildList((List<?>)value) ;
		else if(value instanceof Map) 
			this.buildMap((Map<?,?>)value) ;
		else if(value instanceof Integer)
			this.buildNumber((Integer)value) ;
		else if(value instanceof Short)
			this.buildNumber((Short)value) ;
		else if(value instanceof Float)
			this.buildNumber((Float)value) ;
		else if(value instanceof Character)
			this.buildNumber((Character)value) ;
		else if(value instanceof Long)
			this.buildNumber((Long)value) ;
		else if(value instanceof Double)
			this.buildNumber((Double)value) ;
		else if(value instanceof String)
			this.buildString((String)value) ;
		else
			this.buildNothing(); 
	}
	
	public void buildList(List<?> value) {
		this.beginArray()  ;
		for(int idx=0; idx<value.size(); idx++) {
			Object item = value.get(idx) ;
			if(idx != 0)
				this.addSeparator() ;
			this.buildObject(item) ;
		}
		this.endArray() ;
	}
	
	public void buildMap(Map<?,?> value) {
		this.beginObject() ;
		int index = 0 ;
		Set<?> keys = (Set<?>)value.keySet() ;
		for(Object key : keys) {
			Object val = value.get(key) ;
			if(index!=0)
				this.addSeparator() ;
			this.addString(String.format("\"%s\":", (String)key)) ;
			this.buildObject(val) ;
			index ++ ;
		}
		this.endObject() ;
	}
	
	private void buildNumber(Object value) {
		this.addString(String.valueOf(value)) ;
	}
	
	private void buildString(String value) {
		this.addString(String.format("\"%s\"", value)) ;
	}
	
	private void buildNothing() {
		this.addString("invalid content!!");
	}
	
	private void beginArray() {
		try {
			this.writer.append('[') ;
		}
		catch(Exception e) {
			e.printStackTrace() ;
		}
	}
	
	private void endArray() {
		try {
			this.writer.append(']') ;
		}
		catch(Exception e) {
			e.printStackTrace() ;
		}
	}
	
	private void beginObject() {
		try {
			this.writer.append('{') ;
		}
		catch(Exception e) {
			e.printStackTrace() ;
		}
	}
	
	private void endObject() {
		try {
			this.writer.append('}') ;
		}
		catch(Exception e) {
			e.printStackTrace() ;
		}
	}
	
	private void addSeparator() {
		try {
			this.writer.append(',') ;
		}
		catch(Exception e) {
			e.printStackTrace() ;
		}
	}
	
	private void addString(String value) {
		try {
			this.writer.append(value) ;
		}
		catch(Exception e) {
			e.printStackTrace() ;
		}
	}
}
