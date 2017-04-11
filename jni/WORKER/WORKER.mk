include $(CLEAR_VARS)  
LOCAL_CFLAGS += $(COMPILE_CFLAGS)


#Module Name
LOCAL_MODULE    := WORKER  


#Include Paths
LOCAL_C_INCLUDES := 
LOCAL_EXPORT_C_INCLUDES += 


#Local Library
LOCAL_STATIC_LIBRARIES := NXSDK LEAKER LIBJSON ALGTHM


#Local Library
LOCAL_SHARED_LIBRARIES := 


#Source Files
LOCAL_SRC_FILES := 	WORKER/NXWorker.cpp \
					WORKER/EFRecordWorker.cpp \
					WORKER/EFBuilderMission.cpp \
					WORKER/EFEffectsModel.cpp \
					WORKER/EFEffectsLayer.cpp \
					WORKER/EFEditorWorker.cpp \
					WORKER/EFRenderMission.cpp \
					WORKER/EFExtractMission.cpp \

#cpp flags
LOCAL_CPPFLAGS  += -frtti -fexceptions


#LD flags
LOCAL_LDFLAGS   +=


#System Library
LOCAL_LDLIBS := -lz -landroid -lEGL -llog -lGLESv2 -lOpenSLES -lOpenMAXAL


#ffmpeg Library
LOCAL_LDLIBS +=	$(USER_LIB_FILES)
				

#android support
LOCAL_WHOLE_STATIC_LIBRARIES += android_support


include $(BUILD_SHARED_LIBRARY)  