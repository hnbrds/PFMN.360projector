# -*- mode: cmake; -*-
cmake_minimum_required(VERSION 2.8)
project(OgreFltk)
find_package(OpenCV REQUIRED)

if(APPLE)
	# sudo port install glfw freeglut
	include_directories("/usr/local/include")
  #set(OpenCV_LIBS opencv_core opencv_imgproc opencv_calib3d opencv_video opencv_features2d opencv_ml opencv_highgui opencv_objdetect opencv_contrib opencv_legacy opencv_gpu)
endif()
if(UNIX)
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG -Wall")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -Wall")
  #set(OpenCV_LIBS opencv_core opencv_imgproc opencv_calib3d opencv_video opencv_features2d opencv_ml opencv_highgui opencv_objdetect opencv_contrib opencv_legacy opencv_gpu)
else()
  include_directories("${OgreFltk_SOURCE_DIR}/glfw-3.1.2/include")
endif()

add_executable(OgreFltk
"cv/VideoFrame.cpp"
"cv/VideoFrame.h"
"WaveFrontOBJ.cpp"
"WaveFrontOBJ.h"
"SphericalPlayer.cpp"
"math/math_macro.h"
"math/mathclass.h"
"math/matrix4.h"
"math/quater.h"
"math/vector3.h"
"math/vector4.h"
"math/matrix4.cpp"
"math/quater.cpp"
"math/vector3.cpp"
"math/vector4.cpp"
"math/intersectionTest.cpp"
"math/intersectionTest.h"
"math/interval.cpp"
"math/interval.h"
)

if(UNIX)
	if(APPLE)
	target_link_libraries(OgreFltk
			/System/Library/Frameworks/CoreFoundation.framework
			/System/Library/Frameworks/IOKit.framework
			/System/Library/Frameworks/Cocoa.framework
			/System/Library/Frameworks/OpenGL.framework
			/usr/local/lib/libglfw.dylib
			#/opt/local/lib/libglut.dylib
			/System/Library/Frameworks/GLUT.framework
			/usr/local/lib/libfreeimage.dylib
      "${OpenCV_LIBS}"
      )
	else()
	target_link_libraries(OgreFltk
		GL
		GLU
		glfw
		)
	endif()
else()
target_link_libraries(OgreFltk
	OpenGL32.lib
	GLu32.lib
	"${OgreFltk_SOURCE_DIR}/glfw-3.1.2/lib-msvc110/GLFW.lib"
	"${OpenCV_LIBS}"
	)
endif()
