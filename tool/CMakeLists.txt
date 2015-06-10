cmake_minimum_required( VERSION 3.2 )
project( recolor-tool )

find_package( DevIL REQUIRED )
include_directories( ${IL_INCLUDE_DIR} )
list( APPEND libs ${IL_LIBRARIES} ${ILU_LIBRARIES} )

include_directories( ${recolor_SOURCE_DIR} )

add_executable( ${PROJECT_NAME}
  recolor-tool.cpp
  "$<TARGET_OBJECTS:recolor_object>"
)

target_link_libraries( ${PROJECT_NAME} ${libs} )

file(GLOB ALL_SOURCES "*.h" "*.hpp" "*.c" "*.cpp" "*.m" "*.mm" )
source_group( \\ FILES ${ALL_SOURCES} )
