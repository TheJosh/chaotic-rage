LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := bullet

LOCAL_C_INCLUDES := $(LOCAL_PATH)/src

LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,, \
	$(wildcard $(LOCAL_PATH)/src/BulletCollision/BroadphaseCollision/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/BulletCollision/CollisionDispatch/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/BulletCollision/CollisionShapes/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/BulletCollision/Gimpact/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/BulletCollision/NarrowPhaseCollision/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/BulletDynamics/Character/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/BulletDynamics/ConstraintSolver/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/BulletDynamics/Dynamics/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/BulletDynamics/Vehicle/*.cpp) \
	$(wildcard $(LOCAL_PATH)/src/LinearMath/*.cpp) \
	)
	
include $(BUILD_SHARED_LIBRARY)
