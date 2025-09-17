cmake_minimum_required(VERSION 3.50)

function(copy_dlls TARGET_NAME)
    add_custom_command (
        TARGET "${TARGET_NAME}" POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy -t "$<TARGET_FILE_DIR:${TARGET_NAME}>"
                "$<TARGET_RUNTIME_DLLS:${TARGET_NAME}>" USES_TERMINAL COMMAND_EXPAND_LISTS
    )
endfunction()

set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")

set(PROJECT_NAME cppagent)
project(${PROJECT_NAME})

set(HEADER_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(THIRDPARTY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

include_directories(AFTER "${THIRDPARTY_ROOT}/")

#######################################################
# mimalloc
set(MIMALLOC_ROOT ${THIRDPARTY_ROOT}/mimalloc)
set(MIMALLOC_INCLUDE ${MIMALLOC_ROOT}/include)
set(MIMALLOC_LIB ${MIMALLOC_ROOT}/lib)

include_directories(AFTER ${MIMALLOC_INCLUDE})
link_directories(AFTER ${MIMALLOC_LIB})

add_library(MIMALLOC MODULE IMPORTED)
if(MSVC)
    set_target_properties(MIMALLOC PROPERTIES IMPORTED_LOCATION_DEBUG "mimallocd.lib" IMPORTED_LOCATION_RELEASE "mimalloc.lib")
else()
    set_target_properties(MIMALLOC PROPERTIES IMPORTED_LOCATION "mimalloc.a" IMPORTED_LOCATION_RELEASE "mimalloc.a")
endif()

#######################################################
# replxx
set(REPLXX_ROOT ${THIRDPARTY_ROOT}/replxx)
set(REPLXX_INCLUDE ${REPLXX_ROOT}/include)
set(REPLXX_LIB ${REPLXX_ROOT}/lib)

include_directories(AFTER ${REPLXX_INCLUDE})
link_directories(AFTER ${REPLXX_LIB})

add_library(REPLXX MODULE IMPORTED)
if(MSVC)
    set_target_properties(REPLXX PROPERTIES IMPORTED_LOCATION_DEBUG "replxx-static-d.lib" IMPORTED_LOCATION_RELEASE "replxx-static.lib")
else()
    set_target_properties(REPLXX PROPERTIES IMPORTED_LOCATION "replxx-static-d.a" IMPORTED_LOCATION_RELEASE "replxx-static.a")
endif()

#######################################################
# quill
set(QUILL_ROOT ${THIRDPARTY_ROOT}/quill)
set(QUILL_INCLUDE ${QUILL_ROOT}/include)

include_directories(AFTER ${QUILL_INCLUDE})

#######################################################
# OpenSSL
find_package("openssl" HINTS "${THIRDPARTY_ROOT}/openssl" REQUIRED)
message("openssl ${openssl_FOUND} ${OPENSSL_INCLUDE_DIR}")

#######################################################
# zlib
set(ZLIB_ROOT ${THIRDPARTY_ROOT}/zlib)
set(ZLIB_INCLUDE ${ZLIB_ROOT}/include)
set(ZLIB_LIB ${ZLIB_ROOT}/lib)

include_directories(AFTER ${ZLIB_INCLUDE})
link_directories(AFTER ${ZLIB_LIB})

add_library(ZLIB MODULE IMPORTED)
if(MSVC)
    set_target_properties(ZLIB PROPERTIES IMPORTED_LOCATION_DEBUG "zlibstaticd.lib" IMPORTED_LOCATION_RELEASE "zlibstatic.lib")
else()
    set_target_properties(ZLIB PROPERTIES IMPORTED_LOCATION "zlibstaticd.a" IMPORTED_LOCATION_RELEASE "zlibstatic.a")
endif()

#######################################################
# argparse
set(ARGPARSE_ROOT ${THIRDPARTY_ROOT}/argparse)
set(ARGPARSE_INCLUDE ${ARGPARSE_ROOT}/include)
include_directories(AFTER ${ARGPARSE_INCLUDE})

########################################################################
# Sources
set(HEADERS
    "${HEADER_ROOT}/array.h"
    "${HEADER_ROOT}/stringbuilder.h"
    "${HEADER_ROOT}/utility.h"
    "${HEADER_ROOT}/httplib.h"
    "${HEADER_ROOT}/threadpool.h"
    "${HEADER_ROOT}/cppjson.h"
    "${HEADER_ROOT}/cpptoml.h"
    "${HEADER_ROOT}/log.h"
    "${HEADER_ROOT}/readline.h"
    "${HEADER_ROOT}/settings.h"
    "${HEADER_ROOT}/tool.h"
    "${HEADER_ROOT}/context.h"
    "${HEADER_ROOT}/apiprovider.h"
    "${HEADER_ROOT}/openaiprovider.h"
    "${HEADER_ROOT}/terminal.h"
    "${HEADER_ROOT}/agent.h"
    "${HEADER_ROOT}/app.h"
    "${HEADER_ROOT}/cppagent.h")
set(SOURCES
    "${SOURCE_ROOT}/utility.cpp"
    "${SOURCE_ROOT}/httplib.cpp"
    "${SOURCE_ROOT}/threadpool.cpp"
    "${SOURCE_ROOT}/cppjson.cpp"
    "${SOURCE_ROOT}/cpptoml.cpp"
    "${SOURCE_ROOT}/log.cpp"
    "${SOURCE_ROOT}/readline.cpp"
    "${SOURCE_ROOT}/settings.cpp"
    "${SOURCE_ROOT}/tool.cpp"
    "${SOURCE_ROOT}/context.cpp"
    "${SOURCE_ROOT}/apiprovider.cpp"
    "${SOURCE_ROOT}/openaiprovider.cpp"
    "${SOURCE_ROOT}/terminal.cpp"
    "${SOURCE_ROOT}/agent.cpp"
    "${SOURCE_ROOT}/cppagent.cpp"
    "${SOURCE_ROOT}/app.cpp"
    "${SOURCE_ROOT}/main.cpp")

source_group("include" FILES ${HEADERS})
source_group("src" FILES ${SOURCES})

set(FILES ${HEADERS} ${SOURCES})

set(OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")

add_executable(${PROJECT_NAME} ${FILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

if(MSVC)
    set(DEFAULT_CXX_FLAGS "/DWIN32 /D_WINDOWS /D_MSBC /DCPPHTTPLIB_OPENSSL_SUPPORT /DCPPHTTPLIB_ZLIB_SUPPORT /DREPLXX_STATIC /DQUILL_DISABLE_NON_PREFIXED_MACROS /W4 /WX- /nologo /fp:precise /arch:AVX2 /Zc:wchar_t /TP /Gd")
    if("1800" VERSION_LESS MSVC_VERSION)
        set(DEFAULT_CXX_FLAGS "${DEFAULT_CXX_FLAGS} /EHsc")
    endif()

    set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /RTC1 /Gy /GR- /GS /Gm- /DQUILL_COMPILE_ACTIVE_LOG_LEVEL=0")
    set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /GL /GR- /DNDEBUG /DQUILL_COMPILE_ACTIVE_LOG_LEVEL=7")
    target_link_libraries(${PROJECT_NAME} Ws2_32.lib MIMALLOC OpenSSL::Crypto OpenSSL::SSL ZLIB REPLXX)

elseif(UNIX)
    set(DEFAULT_CXX_FLAGS "-Wall -O0 -g -DCPPHTTPLIB_OPENSSL_SUPPORT -DCPPHTTPLIB_ZLIB_SUPPORT -std=c++20 -std=gnu++20 -march=native -DREPLXX_STATIC -DQUILL_DISABLE_NON_PREFIXED_MACROS")
    set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "-DDQUILL_COMPILE_ACTIVE_LOG_LEVEL=0")
    set(CMAKE_CXX_FLAGS_RELEASE "-DDQUILL_COMPILE_ACTIVE_LOG_LEVEL=7")
    target_link_libraries(${PROJECT_NAME} MIMALLOC OpenSSL::Crypto OpenSSL::SSL ZLIB REPLXX)
elseif(APPLE)
endif()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        OUTPUT_NAME_DEBUG "${PROJECT_NAME}" OUTPUT_NAME_RELEASE "${PROJECT_NAME}"
        VS_DEBUGGER_WORKING_DIRECTORY "${OUTPUT_DIRECTORY}")
copy_dlls(${PROJECT_NAME})
