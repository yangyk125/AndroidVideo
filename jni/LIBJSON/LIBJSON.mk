include $(CLEAR_VARS)  
LOCAL_CFLAGS += $(COMPILE_CFLAGS)


#Module Name
LOCAL_MODULE    := LIBJSON  


#Include Paths
LOCAL_C_INCLUDES := $(LOCAL_PATH)/LIBJSON/include \


#Export Paths
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/LIBJSON/include \


#Local Library
LOCAL_STATIC_LIBRARIES := 


#Local Library
LOCAL_SHARED_LIBRARIES := 


#Module Files
LOCAL_SRC_FILES :=  LIBJSON/src/json_reader.cpp \
					LIBJSON/src/json_value.cpp \
					LIBJSON/src/json_writer.cpp \
				   
				   
#System Library			 
LOCAL_LDLIBS := -landroid -llog


#ffmpeg Library
LOCAL_LDLIB  +=	


include $(BUILD_STATIC_LIBRARY)  