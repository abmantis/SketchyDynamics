LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    	:= sketchydynamics
LOCAL_CPPFLAGS		+= -Wno-write-strings
LOCAL_C_INCLUDES 	+= $(LOCAL_PATH)/../../../include
LOCAL_SRC_FILES 	:= \
	../../../dependecies/CALI/CIAlpha.cxx \
	../../../dependecies/CALI/CICircle.cxx \
	../../../dependecies/CALI/CICommand.cxx \
	../../../dependecies/CALI/CIDiamond.cxx \
	../../../dependecies/CALI/CIEllipse.cxx \
	../../../dependecies/CALI/CIEvaluate.cxx \
	../../../dependecies/CALI/CIFeatures.cxx \
	../../../dependecies/CALI/CIFunction.cxx \
	../../../dependecies/CALI/CIFuzzyNode.cxx \
	../../../dependecies/CALI/CIFuzzySet.cxx \
	../../../dependecies/CALI/CILine.cxx \
	../../../dependecies/CALI/CIPoint.cxx \
	../../../dependecies/CALI/CIPolygon.cxx \
	../../../dependecies/CALI/CIRecognizer.cxx \
	../../../dependecies/CALI/CIRectangle.cxx \
	../../../dependecies/CALI/CIScribble.cxx \
	../../../dependecies/CALI/CIShape.cxx \
	../../../dependecies/CALI/CIStroke.cxx \
	../../../dependecies/CALI/CITriangle.cxx \
	../../../dependecies/CALI/CIUnknown.cxx \
	../../../dependecies/CALI/CIVector.cxx \
	../../../dependecies/CALI/CIWavyLine.cxx \
	../../../dependecies/poly2tri/common/shapes.cc \
	../../../dependecies/poly2tri/sweep/advancing_front.cc \
	../../../dependecies/poly2tri/sweep/cdt.cc \
	../../../dependecies/poly2tri/sweep/sweep.cc \
	../../../dependecies/poly2tri/sweep/sweep_context.cc \
	../../../dependecies/xmlParser/xmlParser.cpp \
	../../../src/SketchyDynamicsAnimatedPolygon.cpp \
	../../../src/SketchyDynamicsApplicationWindow_WGL.cpp \
	../../../src/SketchyDynamicsCore.cpp \
	../../../src/SketchyDynamicsGuessesList.cpp \
	../../../src/SketchyDynamicsInputListener.cpp \
	../../../src/SketchyDynamicsLogger.cpp \
	../../../src/SketchyDynamicsApplicationWindow.cpp \
	../../../src/SketchyDynamicsMaterialManager.cpp \
	../../../src/SketchyDynamicsMatrix3.cpp \
	../../../src/SketchyDynamicsPhysicsBody.cpp \
	../../../src/SketchyDynamicsPhysicsJoint.cpp \
	../../../src/SketchyDynamicsPhysicsManager.cpp \
	../../../src/SketchyDynamicsPolygon.cpp \
	../../../src/SketchyDynamicsRenderer.cpp \
	../../../src/SketchyDynamicsShader_GL.cpp \
	../../../src/SketchyDynamicsVector2.cpp

include $(BUILD_SHARED_LIBRARY)
