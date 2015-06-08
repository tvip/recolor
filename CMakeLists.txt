cmake_minimum_required( VERSION 3.2 )
project( recolor )

set( CMAKE_CXX_STANDARD 98 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

find_package( GLM REQUIRED )
include_directories( ${GLM_INCLUDE_DIRS} )

find_package( DevIL REQUIRED )
list( APPEND libs ${IL_LIBRARIES} ${ILU_LIBRARIES} )

find_package( Boost COMPONENTS system thread log unit_test_framework REQUIRED )
add_definitions( -DBOOST_LOG_DYN_LINK=1 )
list( APPEND libs ${Boost_LIBRARIES} )

find_library( Foundation_LIBRARY Foundation )
list( APPEND libs ${Foundation_LIBRARY} )

set( RES
  res/icons.png
  res/lowres.png
)

add_executable( ${PROJECT_NAME} MACOSX_BUNDLE
  main.cpp
  filesystem.hpp filesystem.mm
  ${RES}
)

target_link_libraries( ${PROJECT_NAME} ${libs} )

set_source_files_properties(
  ${RES}
  PROPERTIES
  MACOSX_PACKAGE_LOCATION Resources
)

message( STATUS "libs:" )
foreach( _lib ${libs} )
    message( STATUS "  ${_lib}" )
endforeach()
