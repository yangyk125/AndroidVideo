package com.android.video.page.effects;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import android.xml.plist.domain.Dict;

import com.android.video.tools.E4FunTool;
import com.android.video.tools.EFPlistParser;

public class EFEffectDataStructs {
	
	public static class EFEffectDataItem {
		
		private static int EFFECT_KEY_GENERATOR = 1 ;
		
		private String effectId   = null ;
		private int	   effectKey  = 0 ;
		
		private String effectName = null ;
		private String effectIcon = null ;
		private String effectPath = null ;
		private Dict   effectDict = null ;
		
		private float  effectWidth = 0.0f ;
		private float  effectHeight = 0.0f ;
		
		private float  effectStart = 0.0f ;
		private float  effectDuration = 0.0f ;
		
		public EFEffectDataItem() {
			effectKey = EFFECT_KEY_GENERATOR ++ ;
		}
		public String getEffectId() {
			return effectId;
		}
		public void setEffectId(String effectId) {
			this.effectId = effectId;
		}
		public int getEffectKey() {
			return effectKey;
		}
		public String getEffectName() {
			return effectName;
		}
		public void setEffectName(String effectName) {
			this.effectName = effectName;
		}
		public String getEffectIcon() {
			return effectIcon;
		}
		public void setEffectIcon(String effectIcon) {
			this.effectIcon = effectIcon;
		}
		public String getEffectPath() {
			return effectPath;
		}
		public void setEffectPath(String effectPath) {
			this.effectPath = effectPath;
		}
		public Dict getEffectDict() {
			return effectDict;
		}
		public void setEffectDict(Dict effectDict) {
			this.effectDict = effectDict;
		}
		public float getEffectWidth() {
			return effectWidth;
		}
		public void setEffectWidth(float effectWidth) {
			this.effectWidth = effectWidth;
		}
		public float getEffectHeight() {
			return effectHeight;
		}
		public void setEffectHeight(float effectHeight) {
			this.effectHeight = effectHeight;
		}
		public float getEffectStart() {
			return effectStart;
		}
		public void setEffectStart(float effectStart) {
			this.effectStart = effectStart;
		}
		public float getEffectDuration() {
			return effectDuration;
		}
		public void setEffectDuration(float effectDuration) {
			this.effectDuration = effectDuration;
		}

	}
	
	
	private List<EFEffectDataItem>	effectDataItems = new ArrayList<EFEffectDataItem>() ;
	private List<Map<?,?>>			effectMapItems  = new ArrayList<Map<?,?>>();
	private float					videoDuration   = 0.0f ;

	public EFEffectDataStructs(float videoDuration) {
		this.videoDuration = videoDuration ;
	}
	
	public float getVideoDuration() {
		return this.videoDuration ;
	}
	
	public void addEffectItem(float seconds, Map<?,?> effectData, String cacheFolder) {
		
		EFEffectDataItem dataItem = new EFEffectDataItem();

		dataItem.setEffectIcon((String)effectData.get("effect_icon"));
		dataItem.setEffectName((String)effectData.get("effect_name"));
		dataItem.setEffectPath(cacheFolder);
		
		Dict config = (Dict)EFPlistParser.parsePlistWithFile(new File(String.format("%s/effect.plist", cacheFolder))) ;
		dataItem.setEffectDict(config) ;
		
		float width = config.getConfigurationNumber("width") ;
		float height = config.getConfigurationNumber("height") ;
		float duration = config.getConfigurationNumber("duration") ;
		if(duration == 0.0f)
			duration = Float.MAX_VALUE ;
		
		dataItem.setEffectWidth(width);
		dataItem.setEffectHeight(height);
		
		dataItem.setEffectStart(seconds) ;
		dataItem.setEffectDuration(Math.min(duration, videoDuration-seconds)) ;
		
		this.effectDataItems.add(dataItem);
		this.effectMapItems.add(effectData) ;
	}
	
	public void delEffectItem(int effectKey) {
		int index = -1 ;
		for(int idx=0; idx<this.effectDataItems.size(); idx++) {
			EFEffectDataItem dataItem = this.effectDataItems.get(idx) ;
			if(dataItem.getEffectKey() == effectKey) {
				index = idx ;
				break ;
			}
		}
		if(index >= 0) {
			this.effectDataItems.remove(index) ;
			this.effectMapItems.remove(index) ;
		}
	}
	
	public void delEffectItems() {
		this.effectDataItems.clear();
		this.effectMapItems.clear();
	}
	
	public String  getStructsJson() {
		Map<String, Object> jsonMap = new HashMap<String, Object>();
		List<Object>	jsonEffects = new ArrayList<Object>() ;
		for(int idx=0; idx<this.effectDataItems.size(); idx++) {
			EFEffectDataItem dataItem = this.effectDataItems.get(idx) ;
			Map<String, Object> effectData = new HashMap<String, Object>() ;
			effectData.put("effect_id", "unknown") ;
			effectData.put("effect_key", dataItem.getEffectKey()) ;
			effectData.put("effect_name", dataItem.getEffectName()) ;
			effectData.put("effect_path", dataItem.getEffectPath()) ;
			effectData.put("effect_width", dataItem.getEffectWidth()) ;
			effectData.put("effect_height", dataItem.getEffectHeight()) ;
			effectData.put("effect_start", dataItem.getEffectStart()) ;
			effectData.put("effect_duration", dataItem.getEffectDuration()) ;
			jsonEffects.add(effectData) ;
		}
		jsonMap.put("effects", jsonEffects) ;
		return E4FunTool.buildJson(jsonMap) ;
	}
	
	public int  getEffectDataItemCount() {
		return this.effectDataItems.size() ;
	}
	
	public EFEffectDataItem getEffectDataItem(int index) {
		return this.effectDataItems.get(index) ;
	}
}
