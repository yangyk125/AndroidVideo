include $(CLEAR_VARS)


LOCAL_CFLAGS += $(COMPILE_CFLAGS)


LOCAL_MODULE := COCOS2DX


LOCAL_C_INCLUDES := $(LOCAL_PATH)/COCOS2DX \
                    $(LOCAL_PATH)/COCOS2DX/include \
                    $(LOCAL_PATH)/COCOS2DX/kazmath/include \
                    $(LOCAL_PATH)/COCOS2DX/platform/android
                    
LOCAL_C_INCLUDES += $(LOCAL_PATH)/LIBRARY/png/include \
       				$(LOCAL_PATH)/LIBRARY/jpeg/include \
					$(LOCAL_PATH)/LIBRARY/curl/include \
					$(LOCAL_PATH)/LIBRARY/tiff/include 
                    
LOCAL_EXPORT_C_INCLUDES :=  $(LOCAL_PATH)/COCOS2DX \
                    		$(LOCAL_PATH)/COCOS2DX/include \
                   			$(LOCAL_PATH)/COCOS2DX/kazmath/include \
                    		$(LOCAL_PATH)/COCOS2DX/platform/android \


LOCAL_CPPFLAGS  += -frtti -fexceptions

                    
LOCAL_SRC_FILES := \
		COCOS2DX/CCConfiguration.cpp \
		COCOS2DX/CCScheduler.cpp \
		COCOS2DX/CCCamera.cpp \
		COCOS2DX/actions/CCAction.cpp \
		COCOS2DX/actions/CCActionCamera.cpp \
		COCOS2DX/actions/CCActionCatmullRom.cpp \
		COCOS2DX/actions/CCActionEase.cpp \
		COCOS2DX/actions/CCActionGrid.cpp \
		COCOS2DX/actions/CCActionGrid3D.cpp \
		COCOS2DX/actions/CCActionInstant.cpp \
		COCOS2DX/actions/CCActionInterval.cpp \
		COCOS2DX/actions/CCActionManager.cpp \
		COCOS2DX/actions/CCActionPageTurn3D.cpp \
		COCOS2DX/actions/CCActionProgressTimer.cpp \
		COCOS2DX/actions/CCActionTiledGrid.cpp \
		COCOS2DX/actions/CCActionTween.cpp \
		COCOS2DX/base_nodes/CCAtlasNode.cpp \
		COCOS2DX/base_nodes/CCNode.cpp \
		COCOS2DX/cocoa/CCAffineTransform.cpp \
		COCOS2DX/cocoa/CCGeometry.cpp \
		COCOS2DX/cocoa/CCAutoreleasePool.cpp \
		COCOS2DX/cocoa/CCDictionary.cpp \
		COCOS2DX/cocoa/CCNS.cpp \
		COCOS2DX/cocoa/CCObject.cpp \
		COCOS2DX/cocoa/CCSet.cpp \
		COCOS2DX/cocoa/CCString.cpp \
		COCOS2DX/cocoa/CCZone.cpp \
		COCOS2DX/cocoa/CCArray.cpp \
		COCOS2DX/cocoa/CCDataVisitor.cpp \
		COCOS2DX/cocos2d.cpp \
		COCOS2DX/CCDirector.cpp \
		COCOS2DX/draw_nodes/CCDrawingPrimitives.cpp \
		COCOS2DX/draw_nodes/CCDrawNode.cpp \
		COCOS2DX/effects/CCGrabber.cpp \
		COCOS2DX/effects/CCGrid.cpp \
		COCOS2DX/kazmath/src/aabb.c \
		COCOS2DX/kazmath/src/mat3.c \
		COCOS2DX/kazmath/src/mat4.c \
		COCOS2DX/kazmath/src/neon_matrix_impl.c \
		COCOS2DX/kazmath/src/plane.c \
		COCOS2DX/kazmath/src/quaternion.c \
		COCOS2DX/kazmath/src/ray2.c \
		COCOS2DX/kazmath/src/utility.c \
		COCOS2DX/kazmath/src/vec2.c \
		COCOS2DX/kazmath/src/vec3.c \
		COCOS2DX/kazmath/src/vec4.c \
		COCOS2DX/kazmath/src/GL/mat4stack.c \
		COCOS2DX/kazmath/src/GL/matrix.c \
		COCOS2DX/keypad_dispatcher/CCKeypadDelegate.cpp \
		COCOS2DX/keypad_dispatcher/CCKeypadDispatcher.cpp \
		COCOS2DX/label_nodes/CCLabelAtlas.cpp \
		COCOS2DX/label_nodes/CCLabelBMFont.cpp \
		COCOS2DX/label_nodes/CCLabelTTF.cpp \
		COCOS2DX/scene_node/CCLayer.cpp \
		COCOS2DX/scene_node/CCScene.cpp \
		COCOS2DX/menu_nodes/CCMenu.cpp \
		COCOS2DX/menu_nodes/CCMenuItem.cpp \
		COCOS2DX/misc_nodes/CCClippingNode.cpp \
		COCOS2DX/misc_nodes/CCMotionStreak.cpp \
		COCOS2DX/misc_nodes/CCProgressTimer.cpp \
		COCOS2DX/misc_nodes/CCRenderTexture.cpp \
		COCOS2DX/particle_nodes/CCParticleExamples.cpp \
		COCOS2DX/particle_nodes/CCParticleSystem.cpp \
		COCOS2DX/particle_nodes/CCParticleBatchNode.cpp \
		COCOS2DX/particle_nodes/CCParticleSystemQuad.cpp \
		COCOS2DX/platform/CCSAXParser.cpp \
		COCOS2DX/platform/CCThread.cpp \
		COCOS2DX/platform/CCFileUtils.cpp \
		COCOS2DX/platform/platform.cpp \
		COCOS2DX/platform/CCEGLViewProtocol.cpp \
		COCOS2DX/platform/android/CCAccelerometer.cpp \
		COCOS2DX/platform/android/CCApplication.cpp \
		COCOS2DX/platform/android/CCCommon.cpp \
		COCOS2DX/platform/android/CCDevice.cpp \
		COCOS2DX/platform/android/CCEGLView.cpp \
		COCOS2DX/platform/android/CCFileUtilsAndroid.cpp \
		COCOS2DX/platform/android/CCImage.cpp \
		COCOS2DX/script_support/CCScriptSupport.cpp \
		COCOS2DX/shaders/ccShaders.cpp \
		COCOS2DX/shaders/CCGLProgram.cpp \
		COCOS2DX/shaders/ccGLStateCache.cpp \
		COCOS2DX/shaders/CCShaderCache.cpp \
		COCOS2DX/sprite_nodes/CCAnimation.cpp \
		COCOS2DX/sprite_nodes/CCAnimationCache.cpp \
		COCOS2DX/sprite_nodes/CCSprite.cpp \
		COCOS2DX/sprite_nodes/CCSpriteBatchNode.cpp \
		COCOS2DX/sprite_nodes/CCSpriteFrame.cpp \
		COCOS2DX/sprite_nodes/CCSpriteFrameCache.cpp \
		COCOS2DX/support/ccUTF8.cpp \
		COCOS2DX/support/CCNotificationCenter.cpp \
		COCOS2DX/support/CCProfiling.cpp \
		COCOS2DX/support/CCPointExtension.cpp \
		COCOS2DX/support/TransformUtils.cpp \
		COCOS2DX/support/base64.cpp \
		COCOS2DX/support/ccUtils.cpp \
		COCOS2DX/support/CCVertex.cpp \
		COCOS2DX/support/data_support/ccCArray.cpp \
		COCOS2DX/support/image_support/TGAlib.cpp \
		COCOS2DX/support/tinyxml2/tinyxml2.cpp \
		COCOS2DX/support/zip_support/ZipUtils.cpp \
		COCOS2DX/support/zip_support/ioapi.cpp \
		COCOS2DX/support/zip_support/unzip.cpp \
		COCOS2DX/support/component/CCComponent.cpp \
		COCOS2DX/support/component/CCComponentContainer.cpp \
		COCOS2DX/text_input_node/CCIMEDispatcher.cpp \
		COCOS2DX/text_input_node/CCTextFieldTTF.cpp \
		COCOS2DX/textures/CCTexture2D.cpp \
		COCOS2DX/textures/CCTextureAtlas.cpp \
		COCOS2DX/textures/CCTextureCache.cpp \
		COCOS2DX/textures/CCTextureETC.cpp \
		COCOS2DX/textures/CCTexturePVR.cpp \
		COCOS2DX/tilemap_parallax_nodes/CCParallaxNode.cpp \
		COCOS2DX/tilemap_parallax_nodes/CCTMXLayer.cpp \
		COCOS2DX/tilemap_parallax_nodes/CCTMXObjectGroup.cpp \
		COCOS2DX/tilemap_parallax_nodes/CCTMXTiledMap.cpp \
		COCOS2DX/tilemap_parallax_nodes/CCTMXXMLParser.cpp \
		COCOS2DX/tilemap_parallax_nodes/CCTileMapAtlas.cpp \
		COCOS2DX/touch_dispatcher/CCTouchDispatcher.cpp \
		COCOS2DX/touch_dispatcher/CCTouchHandler.cpp \
		COCOS2DX/touch_dispatcher/CCTouch.cpp


#system library
LOCAL_LDLIBS := -lGLESv2 -llog -lz


#third library
USER_LIB_FILES  +=	$(LOCAL_PATH)/LIBRARY/png/lib/$(TARGET_ARCH)/libpng.a \
					$(LOCAL_PATH)/LIBRARY/jpeg/lib/$(TARGET_ARCH)/libjpeg.a \
					$(LOCAL_PATH)/LIBRARY/tiff/lib/$(TARGET_ARCH)/libtiff.a 

# define the macro to compile through support/zip_support/ioapi.c
LOCAL_CFLAGS += -Wno-psabi -DUSE_FILE32API
LOCAL_EXPORT_CFLAGS += -Wno-psabi -DUSE_FILE32API

include $(BUILD_STATIC_LIBRARY)
