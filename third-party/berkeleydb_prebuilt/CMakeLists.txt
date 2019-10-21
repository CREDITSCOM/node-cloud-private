cmake_minimum_required(VERSION 3.1)

if (CMAKE_BUILD_TYPE MATCHES Debug)
  set(_bdb_prebuilt_config debug)
  set(_bdb_debug_suffix d)
else()
  set(_bdb_prebuilt_config release)
endif()

if(CMAKE_SIZEOF_VOID_P STREQUAL 4)
  set(_bdb_prebuilt_bits x32)
elseif(CMAKE_SIZEOF_VOID_P STREQUAL 8)
  set(_bdb_prebuilt_bits x64)
else()
  set(_bdb_prebuilt_bits unknown)
endif()

if (WIN32)
  set(_bdb_prebuilt_platform windows)
  set(_bdb_lib_name libdb181s${_bdb_debug_suffix})
  set(_bdb_additional_libs Ws2_32)
elseif(UNIX AND NOT APPLE)
  set(_bdb_prebuilt_platform linux)
  set(_bdb_lib_name libdb_cxx-18.1)
  set(_bdb_additional_libs pthread)
elseif(APPLE)
  set(_bdb_prebuilt_platform mac)
  set(_bdb_lib_name libdb_cxx-18.1)
else()
  set(_bdb_prebuilt_platform unknown)
endif()

set(_bdb_prebuilt_folder
  ${CMAKE_CURRENT_LIST_DIR}/${_bdb_prebuilt_platform}/${_bdb_prebuilt_bits}/${_bdb_prebuilt_config})

add_library(BerkeleyDB STATIC IMPORTED GLOBAL)
set_property(TARGET BerkeleyDB PROPERTY IMPORTED_LOCATION
  ${_bdb_prebuilt_folder}/lib/${_bdb_lib_name}${CMAKE_STATIC_LIBRARY_SUFFIX}
  )

set_property(TARGET BerkeleyDB PROPERTY INTERFACE_INCLUDE_DIRECTORIES
  ${_bdb_prebuilt_folder}/include
  )

if(_bdb_additional_libs)
  set_property(TARGET BerkeleyDB PROPERTY INTERFACE_LINK_LIBRARIES
    ${_bdb_additional_libs}
    )
endif()
