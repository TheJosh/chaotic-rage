LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := assimp

LOCAL_CPP_FEATURES += exceptions

LOCAL_C_INCLUDES := $(LOCAL_PATH)/code \
	$(LOCAL_PATH)/code/BoostWorkaround
	
LOCAL_CFLAGS := -DASSIMP_BUILD_BOOST_WORKAROUND

LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/code/*.cpp)) \
	contrib/zlib/adler32.c \
	contrib/zlib/compress.c \
	contrib/zlib/crc32.c \
	contrib/zlib/deflate.c \
	contrib/zlib/inffast.c \
	contrib/zlib/inflate.c \
	contrib/zlib/inftrees.c \
	contrib/zlib/trees.c \
	contrib/zlib/zutil.c \
	contrib/ConvertUTF/ConvertUTF.c \
	contrib/unzip/ioapi.c \
	contrib/unzip/unzip.c \
	contrib/irrXML/irrXML.cpp \
	contrib/clipper/clipper.cpp \
	contrib/poly2tri/poly2tri/common/shapes.cc \
	contrib/poly2tri/poly2tri/sweep/advancing_front.cc \
	contrib/poly2tri/poly2tri/sweep/cdt.cc \
	contrib/poly2tri/poly2tri/sweep/sweep.cc \
	contrib/poly2tri/poly2tri/sweep/sweep_context.cc \
	
include $(BUILD_SHARED_LIBRARY)
