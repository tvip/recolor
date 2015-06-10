cmake_minimum_required( VERSION 3.2 )
project( recolor-app )

find_package( DevIL REQUIRED )
include_directories( ${IL_INCLUDE_DIR} )
list( APPEND libs ${IL_LIBRARIES} ${ILU_LIBRARIES} )

find_library( Foundation_LIBRARY Foundation )
list( APPEND libs ${Foundation_LIBRARY} )

set( RES
  res/colors.txt
  res/icons.png
  res/icons1.bmp
  res/lowres.png
  res/small.png
)

include_directories( ${recolor_SOURCE_DIR} )

add_executable( ${PROJECT_NAME} MACOSX_BUNDLE
  recolor-app.cpp
  filesystem.hpp filesystem.mm
  "$<TARGET_OBJECTS:recolor_object>"
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
