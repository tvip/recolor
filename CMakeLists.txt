cmake_minimum_required( VERSION 3.2 )
project( recolor )

set( CMAKE_CXX_STANDARD 98 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

find_package( GLM REQUIRED )
add_definitions( -DGLM_FORCE_CXX98=1 -DGLM_FORCE_RADIANS=1 )
include_directories( ${GLM_INCLUDE_DIRS} )

find_package( DevIL REQUIRED )
list( APPEND libs ${IL_LIBRARIES} ${ILU_LIBRARIES} )

find_package( Boost COMPONENTS log REQUIRED )
add_definitions( -DBOOST_LOG_DYN_LINK=1 )
list( APPEND libs ${Boost_LIBRARIES} )

find_library( Foundation_LIBRARY Foundation )
list( APPEND libs ${Foundation_LIBRARY} )

set( RES
  res/icons.png
  res/lowres.png
)

set( ${PROJECT_NAME}_SRC
  recolor.hpp recolor.cpp
)
add_library( ${PROJECT_NAME}_object OBJECT ${${PROJECT_NAME}_SRC} )

add_executable( ${PROJECT_NAME} MACOSX_BUNDLE
  main.cpp
  filesystem.hpp filesystem.mm
  "$<TARGET_OBJECTS:${PROJECT_NAME}_object>"
  ${RES}
)

target_link_libraries( ${PROJECT_NAME} ${libs} )

set_source_files_properties(
  ${RES}
  PROPERTIES
  MACOSX_PACKAGE_LOCATION Resources
)

file(GLOB ALL_SOURCES "*.h" "*.hpp" "*.c" "*.cpp" "*.m" "*.mm" )
source_group( \\ FILES ${ALL_SOURCES} )

message( STATUS "libs:" )
foreach( _lib ${libs} )
    message( STATUS "  ${_lib}" )
endforeach()

enable_testing()
include( CTest )

find_package( Boost COMPONENTS log unit_test_framework REQUIRED )
list( APPEND libs ${Boost_LIBRARIES} )

add_executable( test-${PROJECT_NAME} test-recolor.cpp "$<TARGET_OBJECTS:${PROJECT_NAME}_object>" )
add_test( NAME test-recolor COMMAND test-recolor --log_level=all )
target_link_libraries( test-recolor ${libs} )
