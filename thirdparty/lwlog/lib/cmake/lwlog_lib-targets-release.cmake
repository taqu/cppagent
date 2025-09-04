#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "lwlog::lwlog_lib" for configuration "Release"
set_property(TARGET lwlog::lwlog_lib APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(lwlog::lwlog_lib PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/lwlog_lib.lib"
  )

list(APPEND _cmake_import_check_targets lwlog::lwlog_lib )
list(APPEND _cmake_import_check_files_for_lwlog::lwlog_lib "${_IMPORT_PREFIX}/lib/lwlog_lib.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
