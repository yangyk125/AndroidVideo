include $(CLEAR_VARS)  
LOCAL_CFLAGS += $(COMPILE_CFLAGS)


#Module Name
LOCAL_MODULE    := LEAKER  


#Include Paths
LOCAL_C_INCLUDES := $(LOCAL_PATH)/LEAKER \
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/LEAKER	\


#Local Library
LOCAL_STATIC_LIBRARIES := 


#Local Library
LOCAL_SHARED_LIBRARIES := 


#Source Files
LOCAL_SRC_FILES := 	LEAKER/leaker.cpp \


#System Library
LOCAL_LDLIBS := -landroid -llog

#ffmpeg Library
LOCAL_LDLIBS +=	



include $(BUILD_STATIC_LIBRARY)  