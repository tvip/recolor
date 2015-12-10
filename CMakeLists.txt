cmake_minimum_required( VERSION 2.8 )
project( metaproject )

include( ExternalProject )
include( ProcessorCount )

set( TOOLS_DIR ${CMAKE_BINARY_DIR}/Tools )

macro( _ExternalProject_Get_Property _project _property )
  ExternalProject_Get_Property( ${_project} ${_property} )
  set( ${_project}_${_property} ${${_property}} )
endmacro()

set( MAKE_JOBSERVER_FLAGS "" )
ProcessorCount(N)
if(NOT N EQUAL 0)
  set( MAKE_JOBSERVER_FLAGS ${MAKE_JOBSERVER_FLAGS} -j${N} )
endif()

set( HOST_ENV
  PATH=${TOOLS_DIR}/bin:$ENV{PATH}
  PKG_CONFIG_PATH=${TOOLS_DIR}/lib/pkgconfig
  C_INCLUDE_PATH=${TOOLS_DIR}/include
  LD_LIBRARY_PATH=${TOOLS_DIR}/lib
)

ExternalProject_Add( pugixml_download
  URL "https://github.com/zeux/pugixml/releases/download/v1.7/pugixml-1.7.tar.gz"
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -E echo "skip"
  BUILD_COMMAND  ${CMAKE_COMMAND} -E echo "skip"
  INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "skip"
)

_ExternalProject_Get_Property( pugixml_download SOURCE_DIR )

ExternalProject_Add( pugixml
  DEPENDS pugixml_download
  DOWNLOAD_COMMAND ${CMAKE_COMMAND} -E echo "skip"
  SOURCE_DIR "${pugixml_download_SOURCE_DIR}/scripts"
  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_MODULE_PATH=${CMAKE_CURRENT_SOURCE_DIR}/cmake
    -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/Staging
)

#ExternalProject_Add( devil
#  URL "http://downloads.sourceforge.net/project/openil/DevIL/1.7.8/DevIL-1.7.8.tar.gz"
#  CONFIGURE_COMMAND ${HOST_ENV} ./configure --prefix=${TOOLS_DIR}
#  BUILD_COMMAND ${HOST_ENV} ${CMAKE_MAKE_PROGRAM} ${MAKE_JOBSERVER_FLAGS}
#  BUILD_IN_SOURCE 1
#)

ExternalProject_Add( src
  DEPENDS pugixml #devil
  DOWNLOAD_COMMAND ${CMAKE_COMMAND} -E echo "skip"
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src
  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_MODULE_PATH=${CMAKE_CURRENT_SOURCE_DIR}/cmake
    -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/Staging
)

ExternalProject_Add_Step( src forcebuild
  COMMAND ${CMAKE_COMMAND} -E echo "Force build"
  DEPENDEES configure
  DEPENDERS build
  ALWAYS 1
)

#add_subdirectory( test-data )
