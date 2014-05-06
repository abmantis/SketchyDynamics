LOCAL_PATH := $(call my-dir)

LS_CPP:= $(subst $(1)/,,$(wildcard $(1)/$(2)/*.cpp))
BOX2D_CPP:= $(call LS_CPP, $(LOCAL_PATH), Box2D/Collision) \
            $(call LS_CPP, $(LOCAL_PATH), Box2D/Collision/Shapes) \
            $(call LS_CPP, $(LOCAL_PATH), Box2D/Common) \
            $(call LS_CPP, $(LOCAL_PATH), Box2D/Dynamics) \
            $(call LS_CPP, $(LOCAL_PATH), Box2D/Dynamics/Contacts) \
            $(call LS_CPP, $(LOCAL_PATH), Box2D/Dynamics/Joints) \
            $(call LS_CPP, $(LOCAL_PATH), Box2D/Rope)

include $(CLEAR_VARS)

LOCAL_MODULE := box2d_static
LOCAL_SRC_FILES := Box2D/Collision/Shapes/b2CircleShape.cpp \
	Box2D/Collision/Shapes/b2PolygonShape.cpp \
	Box2D/Collision/b2BroadPhase.cpp \
	Box2D/Collision/b2CollideCircle.cpp \
	Box2D/Collision/b2CollidePolygon.cpp \
	Box2D/Collision/b2Collision.cpp \
	Box2D/Collision/b2Distance.cpp \
	Box2D/Collision/b2DynamicTree.cpp \
	Box2D/Collision/b2TimeOfImpact.cpp \
	Box2D/Common/b2BlockAllocator.cpp \
	Box2D/Common/b2Math.cpp \
	Box2D/Common/b2Settings.cpp \
	Box2D/Common/b2StackAllocator.cpp \
	Box2D/Dynamics/Contacts/b2CircleContact.cpp \
	Box2D/Dynamics/Contacts/b2Contact.cpp \
	Box2D/Dynamics/Contacts/b2ContactSolver.cpp \
	Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp \
	Box2D/Dynamics/Contacts/b2PolygonContact.cpp \
	Box2D/Dynamics/Joints/b2DistanceJoint.cpp \
	Box2D/Dynamics/Joints/b2FrictionJoint.cpp \
	Box2D/Dynamics/Joints/b2GearJoint.cpp \
	Box2D/Dynamics/Joints/b2Joint.cpp \
	Box2D/Dynamics/Joints/b2MouseJoint.cpp \
	Box2D/Dynamics/Joints/b2PrismaticJoint.cpp \
	Box2D/Dynamics/Joints/b2PulleyJoint.cpp \
	Box2D/Dynamics/Joints/b2RevoluteJoint.cpp \
	Box2D/Dynamics/Joints/b2WeldJoint.cpp \
	Box2D/Dynamics/b2Body.cpp \
	Box2D/Dynamics/b2ContactManager.cpp \
	Box2D/Dynamics/b2Fixture.cpp \
	Box2D/Dynamics/b2Island.cpp \
	Box2D/Dynamics/b2World.cpp \
	Box2D/Dynamics/b2WorldCallbacks.cpp 

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES := $(LOCAL_EXPORT_C_INCLUDES)

include $(BUILD_STATIC_LIBRARY)
