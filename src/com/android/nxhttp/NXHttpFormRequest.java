package com.android.nxhttp;

import java.util.HashMap;
import java.util.Map;

public class NXHttpFormRequest extends NXHttpRequest {
	
	protected static class NXHttpFormField {
		
		private String 		content ;
		private boolean 	isfile  ;
		
		public NXHttpFormField(String content, boolean isfile) {
			this.content = content;
			this.isfile = isfile;
		}
		public String getContent() {
			return content;
		}
		public void setContent(String content) {
			this.content = content;
		}
		public boolean isIsfile() {
			return isfile;
		}
		public void setIsfile(boolean isfile) {
			this.isfile = isfile;
		}
	}
	
	private Map<String, NXHttpFormField>  formFields = new HashMap<String, NXHttpFormField>() ;

	public void addFormFieldFile(String key, String path) {
		formFields.put(key, new NXHttpFormField(path, true)) ;
	}
	
	public void addFormFieldString(String key, String value) {
		formFields.put(key, new NXHttpFormField(value, false)) ;
	}
	
	public void delFormField(String key) {
		formFields.remove(key) ;
	}
}
