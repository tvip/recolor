cmake_minimum_required( VERSION 2.8 )
project( recolor-tool )

include( IdeFileStructure )

find_package( DevIL REQUIRED )
include_directories( ${IL_INCLUDE_DIR} )
include_directories( ${IL_INCLUDE_DIR}/.. )
list( APPEND libs ${IL_LIBRARIES} ${ILU_LIBRARIES} )
#message( FATAL_ERROR "IL_INCLUDE_DIR ${IL_INCLUDE_DIR}" )

find_package( PugiXML REQUIRED )
message( STATUS "Found pugixml: ${pugixml_DIR}" )
include_directories( ${PUGIXML_INCLUDE_DIR} )
#include_directories( "${pugixml_DIR}/../../../include" )
list( APPEND libs ${PUGIXML_LIBRARY} )

include_directories( ${recolor_SOURCE_DIR} )

add_executable( ${PROJECT_NAME}
  recolor-tool.cpp
  "$<TARGET_OBJECTS:recolor_object>"
)

target_link_libraries( ${PROJECT_NAME} ${libs} )

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets
  RUNTIME DESTINATION bin/
)
