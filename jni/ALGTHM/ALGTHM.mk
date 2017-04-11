include $(CLEAR_VARS)
LOCAL_CFLAGS += $(COMPILE_CFLAGS)

#Module Name
LOCAL_MODULE    := ALGTHM  


#Include Paths
LOCAL_C_INCLUDES := $(LOCAL_PATH)/ALGTHM \
					$(LOCAL_PATH)/ALGTHM/include \

                    
#Export Paths
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/ALGTHM \


#Local Library
LOCAL_STATIC_LIBRARIES := NXSDK


#Local Library
LOCAL_SHARED_LIBRARIES := 


#cpp flags
LOCAL_CPPFLAGS  += -frtti -fexceptions


#Module Files
LOCAL_SRC_FILES :=  ALGTHM/EFALGTracker.cpp \
					ALGTHM/EFALGSaliency.cpp \
				   
				   
#System Library			 
LOCAL_LDLIBS := -landroid -llog


#algthm Library
USER_LIB_FILES  +=	


include $(BUILD_STATIC_LIBRARY)  
