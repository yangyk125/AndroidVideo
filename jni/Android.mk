LOCAL_PATH := $(call my-dir)

#prbuild macro defination
COMPILE_CFLAGS := -DDEBUG
USER_LIB_FILES := 

#show NXBuffer Object count
#COMPILE_CFLAGS += -DPRINT_NXSDK_BUFFER_COUNT


#show NXSDK Object count
#COMPILE_CFLAGS += -DPRINT_NXSDK_OBJECT_COUNT


#show cocos2dx draw scene time
COMPILE_CFLAGS += -DPRINT_COCOS2DX_ANIM_TIME


#show workbeach runloop time
#COMPILE_CFLAGS += -DPRINT_WORKBEACH_LOOP_TIME


#show cocos2dx object count
COMPILE_CFLAGS += -DPRINT_COCOS2DX_OBJECT_COUNT


#show cocos2dx texture count
COMPILE_CFLAGS += -DPRINT_COCOS2DX_TEXTURE_COUNT


#show ffmpeg encode video time
COMPILE_CFLAGS += -DPRINT_ENCODE_VIDEO_TIME


#show ffmpeg decode video time
#COMPILE_CFLAGS += -DPRINT_DECODE_VIDEO_TIME


#show ffmpeg encode audio time
COMPILE_CFLAGS += -DPRINT_ENCODE_AUDIO_TIME


#show ffmpeg decode audio time
#COMPILE_CFLAGS += -DPRINT_DECODE_AUDIO_TIME


#show ffmpeg scale video time
#COMPILE_CFLAGS += -DPRINT_SCALE_VIDEO_TIME


#show ffmpeg resmple audio time
#COMPILE_CFLAGS += -DPRINT_RESMPLE_AUDIO_TIME


#show temp file write time
COMPILE_CFLAGS += -DPRINT_WT_TEMP_VIDEO_TIME


#show temp file read time
COMPILE_CFLAGS += -DPRINT_RD_TEMP_VIDEO_TIME


#show player play_frame time
COMPILE_CFLAGS += -DPRINT_PLAYER_FRAME_TIME


#show track image time
COMPILE_CFLAGS += -DPRINT_TRACK_FRAME_TIME


include $(LOCAL_PATH)/LEAKER/LEAKER.mk
include $(LOCAL_PATH)/COCOS2DX/COCOS2DX.mk
include $(LOCAL_PATH)/NXSDK/NXSDK.mk  
include $(LOCAL_PATH)/ALGTHM/ALGTHM.mk
include $(LOCAL_PATH)/LIBJSON/LIBJSON.mk
include $(LOCAL_PATH)/WORKER/WORKER.mk


$(call import-module, android/support)