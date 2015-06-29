cmake_minimum_required( VERSION 3.2 )
project( recolor-web )

include( IdeFileStructure )

add_executable( dummy
  dummy.cpp
)

install(TARGETS dummy EXPORT ${PROJECT_NAME}Targets
  RUNTIME DESTINATION bin/
)
