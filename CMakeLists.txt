cmake_minimum_required( VERSION 3.2 )
project( recolor VERSION 0.1 )

# setting CMAKE_DISABLE_FIND_PACKAGE_Boost to TRUE force disables using Boost libs
option( ${PROJECT_NAME}_BUILD_TOOL "build command line tool" ON )
option( ${PROJECT_NAME}_BUILD_TESTS "enable unit testing" ON )
option( ${PROJECT_NAME}_BUILD_APP "build Mac OS app bundle" ${APPLE} )
option( ${PROJECT_NAME}_BUILD_SHARED "build shared lib" ON )
option( ${PROJECT_NAME}_BUILD_STATIC "build static lib" ON )

set(ConfigPackageLocation lib/cmake/)
set(INCLUDE_INSTALL_DIR include/ )
set(LIB_INSTALL_DIR lib/ )
set(BINARY_INSTALL_DIR bin/ )
set(INSTALL_DIRS
	LIBRARY DESTINATION ${LIB_INSTALL_DIR}
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
  RUNTIME DESTINATION ${BIN_INSTALL_DIR}
  INCLUDES DESTINATION ${INCLUDE_INSTALL_DIR}
)

set( CMAKE_CXX_STANDARD 98 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )
include( IdeFileStructure )

find_package( GLM REQUIRED )
add_definitions( -DGLM_FORCE_CXX98=1 -DGLM_FORCE_NO_CTOR_INIT=1 -DGLM_FORCE_RADIANS=1 )
include_directories( ${GLM_INCLUDE_DIRS} )
#message( FATAL_ERROR "GLM_INCLUDE_DIRS ${GLM_INCLUDE_DIRS}" )

set( ${PROJECT_NAME}_SRC
  recolor.hpp recolor.cpp
)
add_library( ${PROJECT_NAME}_object OBJECT ${${PROJECT_NAME}_SRC} )

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

install(TARGETS ${PROJECT_NAME}-shared ${PROJECT_NAME}-static EXPORT ${PROJECT_NAME}Targets
  ${INSTALL_DIRS}
)

install( FILES recolor.hpp
  DESTINATION include
  COMPONENT Devel
)

include( CMakePackageConfigHelpers )
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Version.cmake"
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT ${PROJECT_NAME}Targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
)

configure_package_config_file(
	recolor-config.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
	INSTALL_DESTINATION lib/cmake
	PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR BINARY_INSTALL_DIR
)

install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
  DESTINATION ${ConfigPackageLocation}
)

install( FILES
	"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Version.cmake"
  DESTINATION ${ConfigPackageLocation}
)
