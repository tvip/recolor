cmake_minimum_required( VERSION 3.2 )
project( recolor )

option( ${PROJECT_NAME}_BUILD_TOOL "build command line tool" ON )
option( ${PROJECT_NAME}_BUILD_TESTS "enable unit testing" ON )
option( ${PROJECT_NAME}_BUILD_APP "build Mac OS app bundle" ${APPLE} )
option( ${PROJECT_NAME}_BUILD_SHARED "build shared lib" OFF )
option( ${PROJECT_NAME}_BUILD_STATIC "build static lib" OFF )

set( CMAKE_CXX_STANDARD 98 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

find_package( GLM REQUIRED )
add_definitions( -DGLM_FORCE_CXX98=1 -DGLM_FORCE_NO_CTOR_INIT=1 -DGLM_FORCE_RADIANS=1 )
include_directories( ${GLM_INCLUDE_DIRS} )

set( ${PROJECT_NAME}_SRC
  recolor.hpp recolor.cpp
)
add_library( ${PROJECT_NAME}_object OBJECT ${${PROJECT_NAME}_SRC} )

file( GLOB ALL_SOURCES "*.h" "*.hpp" "*.c" "*.cpp" "*.m" "*.mm" )
source_group( \\ FILES ${ALL_SOURCES} )

if ( ${${PROJECT_NAME}_BUILD_TOOL} )
	add_subdirectory( tool )
endif()

if ( ${${PROJECT_NAME}_BUILD_TESTS} )
  enable_testing()
  add_subdirectory( test )
endif()

if ( ${${PROJECT_NAME}_BUILD_APP} )
  add_subdirectory( app )
endif()

if ( ${${PROJECT_NAME}_BUILD_SHARED} )
  add_library( ${PROJECT_NAME}-shared SHARED "$<TARGET_OBJECTS:${PROJECT_NAME}_object>" )
endif()

if ( ${${PROJECT_NAME}_BUILD_STATIC} )
  add_library( ${PROJECT_NAME}-static STATIC "$<TARGET_OBJECTS:${PROJECT_NAME}_object>" )
endif()
