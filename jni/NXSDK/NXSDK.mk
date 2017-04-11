include $(CLEAR_VARS)  
LOCAL_CFLAGS += $(COMPILE_CFLAGS)


#Module Name
LOCAL_MODULE    := NXSDK  


#Include Paths
LOCAL_C_INCLUDES := $(LOCAL_PATH)/NXSDK \


#Include cocos2d path					
LOCAL_C_INCLUDES +=	$(LOCAL_PATH)/COCOS2DX \
                    $(LOCAL_PATH)/COCOS2DX/include \
                    $(LOCAL_PATH)/COCOS2DX/kazmath/include \
                    $(LOCAL_PATH)/COCOS2DX/platform/android \
                    $(LOCAL_PATH)/LIBRARY \
                    $(LOCAL_PATH)/LIBRARY/ffmpeg/include \
					$(LOCAL_PATH)/LIBRARY/faac/include \
					$(LOCAL_PATH)/LIBRARY/lame/include \
					$(LOCAL_PATH)/LIBRARY/x264/include \


#Export Paths
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/NXSDK	\
						   $(LOCAL_PATH)/NXSDK/NXKT	\



#Local Library
LOCAL_STATIC_LIBRARIES := COCOS2DX LEAKER


#Local Library
LOCAL_SHARED_LIBRARIES := 



#Module Files
LOCAL_SRC_FILES := NXSDK/NXSDK.cpp \
				   NXSDK/NXKT/NXKit.cpp \
				   NXSDK/NXKT/NXLock.cpp \
				   NXSDK/NXKT/NXFile.cpp \
				   NXSDK/NXKT/NXEvent.cpp \
				   NXSDK/NXKT/NXTimer.cpp \
				   NXSDK/NXKT/NXBuffer.cpp \
				   NXSDK/NXKT/NXThread.cpp \
				   NXSDK/NXKT/NXThrottle.cpp \
				   NXSDK/NXGL/NXGL.cpp \
				   NXSDK/NXGL/NXGLContext.cpp \
				   NXSDK/NXGL/NXGLSurface.cpp \
				   NXSDK/NXGL/NXGLDrawer.cpp \
				   NXSDK/NXGL/NXGLCanvas.cpp \
				   NXSDK/NXGL/NXGLTrimer.cpp \
				   NXSDK/NXFF/NXFFSWriter.cpp \
				   NXSDK/NXFF/NXFFAWriter.cpp \
				   NXSDK/NXFF/NXFFSReader.cpp \
				   NXSDK/NXTM/NXTMAWriter.cpp \
				   NXSDK/NXTM/NXTMAReader.cpp \
				   NXSDK/NXTM/NXTMSWriter.cpp \
				   NXSDK/NXTM/NXTMSReader.cpp \
				   NXSDK/NXTM/NXTMSShoter.cpp \
				   NXSDK/NXSL/NXSLSpeaker.cpp \
				   NXSDK/NXSL/NXSLRecorder.cpp \
				   NXSDK/NXPL/NXWBPlayer.cpp \
				   NXSDK/NXXL/NXXLCapturer.cpp \
				   NXSDK/NXWB/NXWorkbeach.cpp \
				   NXSDK/NXCC/NXCCContext.cpp \
				   NXSDK/NXCC/NXCCSprite.cpp \
				   NXSDK/NXCC/NXCCNode.cpp \
				   NXSDK/NXCC/NXCCDirector.cpp \
				   NXSDK/NXCC/NXCCShareNode.cpp \
				   NXSDK/NXCC/NXCCSpriteNV12.cpp \
				   
				   
#System Library			 
LOCAL_LDLIBS := -landroid -lEGL -llog -lGLESv2 -lOpenSLES -lOpenMAXAL


#ffmpeg Library
USER_LIB_FILES  +=	$(LOCAL_PATH)/LIBRARY/ffmpeg/lib/$(TARGET_ARCH)/libavformat.a \
					$(LOCAL_PATH)/LIBRARY/ffmpeg/lib/$(TARGET_ARCH)/libavcodec.a \
					$(LOCAL_PATH)/LIBRARY/ffmpeg/lib/$(TARGET_ARCH)/libavdevice.a \
					$(LOCAL_PATH)/LIBRARY/ffmpeg/lib/$(TARGET_ARCH)/libavfilter.a \
					$(LOCAL_PATH)/LIBRARY/ffmpeg/lib/$(TARGET_ARCH)/libavresample.a \
					$(LOCAL_PATH)/LIBRARY/ffmpeg/lib/$(TARGET_ARCH)/libavutil.a \
					$(LOCAL_PATH)/LIBRARY/ffmpeg/lib/$(TARGET_ARCH)/libswscale.a \
					$(LOCAL_PATH)/LIBRARY/ffmpeg/lib/$(TARGET_ARCH)/libpostproc.a \
					$(LOCAL_PATH)/LIBRARY/ffmpeg/lib/$(TARGET_ARCH)/libswresample.a \
                    $(LOCAL_PATH)/LIBRARY/fdkaac/lib/$(TARGET_ARCH)/libfdk-aac.a \
                    $(LOCAL_PATH)/LIBRARY/x264/lib/$(TARGET_ARCH)/libx264.a \


LOCAL_WHOLE_STATIC_LIBRARIES += android_support


include $(BUILD_STATIC_LIBRARY)  