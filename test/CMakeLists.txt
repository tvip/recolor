cmake_minimum_required( VERSION 3.2 )
project( recolor-test )

include( IdeFileStructure )

find_package( Boost COMPONENTS log unit_test_framework )

if ( ${Boost_FOUND} )

  add_definitions( -DBOOST_LOG_DYN_LINK=1 -DBOOST_TEST_DYN_LINK=1 )
  include_directories( ${Boost_INCLUDE_DIRS} )
  list( APPEND libs ${Boost_LIBRARIES} )

  include_directories( ${recolor_SOURCE_DIR} )

  add_executable( ${PROJECT_NAME} test-recolor.cpp "$<TARGET_OBJECTS:recolor_object>" )
  add_test( NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME} --log_level=all )
  target_link_libraries( ${PROJECT_NAME} ${libs} )

else()

  message( WARNING "skip recolor test : Boost not found" )

endif()
