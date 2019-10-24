LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

BUILD_TEST_BIN := false
LOCAL_MODULE := handler


LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SRC_FILES += src/Handler.cpp
LOCAL_SRC_FILES += src/Message.cpp

ifdef TEST
LOCAL_SRC_FILES += src/Main.cpp
BUILD_TEST_BIN = true
endif

$(info "LOCAL_SRC_FILES :"$(LOCAL_SRC_FILES))


#添加系统库依赖
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -lm -llog
LOCAL_CFLAGS += -g -std=c++11

#依赖so 这里直接采用LOCAL_LDLIBS
LOCAL_SHARED_LIBRARIES += $(LOCAL_LDLIBS)

#编译目标为可执行测试bin还是动态so
ifeq ($(BUILD_TEST_BIN), true)
include $(BUILD_EXECUTABLE)
else
include $(BUILD_SHARED_LIBRARY)
endif
