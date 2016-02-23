# Adapted from the work by Felix Kauselmann
# https://github.com/selmf/unarr/blob/master/CMakeLists.txt

cmake_minimum_required(VERSION 2.6)
project(unarr C)

#Set up api and release version for later use.
#Increase in major api version indicates api
#breakage!! For non-breaking changes, use
#minor api version instead.
set (UNARR_API_VERSION_MAJOR 1)
set (UNARR_API_VERSION_MINOR 0)

#set build type to default if unset
if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release CACHE STRING
       "Choose the type of build, options are: None Debug Release RelWithDebInfo
MinSizeRel."
       FORCE )
endif()

# find_package(ZLIB)
# find_package(BZip2)

if (UNIX OR MINGW)
    add_compile_options(-fomit-frame-pointer -D_FILE_OFFSET_BITS=64)
endif (UNIX OR MINGW)

#sources

set (HEADERS
    unarr/common/allocator.h
    unarr/common/unarr-imp.h
    unarr/rar/rar.h
    unarr/rar/lzss.h
    unarr/rar/rarvm.h
    unarr/lzmasdk/LzmaDec.h
    unarr/lzmasdk/Ppmd7.h
    unarr/lzmasdk/CpuArch.h
    unarr/lzmasdk/Ppmd.h
    unarr/lzmasdk/7zTypes.h
    unarr/lzmasdk/Ppmd8.h
    unarr/lzmasdk/Precomp.h
#     _7z/_7z.h
#     zip/zip.h
#     zip/inflate.h
#     tar/tar.h
    )

set (SOURCES
    unarr/rar/uncompress-rar.c
    unarr/rar/huffman-rar.c
    unarr/rar/rar.c
    unarr/rar/filter-rar.c
    unarr/rar/rarvm.c
    unarr/rar/parse-rar.c
#     _7z/_7z.c
#     zip/zip.c
#     zip/inflate.c
#     zip/parse-zip.c
#     zip/uncompress-zip.c
#     tar/tar.c
#     tar/parse-tar.c
    unarr/lzmasdk/Ppmd7.c
    unarr/lzmasdk/Ppmd8.c
    unarr/lzmasdk/CpuArch.c
    unarr/lzmasdk/LzmaDec.c
    unarr/lzmasdk/Ppmd7Dec.c
    unarr/lzmasdk/Ppmd8Dec.c
    unarr/common/custalloc.c
    unarr/common/unarr.c
    unarr/common/stream.c
    unarr/common/conv.c
    unarr/common/crc32.c
    )

#build targets
if(POLICY CMP0063)
    cmake_policy (SET CMP0063 NEW)
endif(POLICY CMP0063)

add_library(unarr STATIC unarr/unarr.h ${HEADERS} ${SOURCES})
target_include_directories(unarr
    PUBLIC
    unarr/
)
set_target_properties(unarr PROPERTIES VERSION ${UNARR_API_VERSION_MAJOR}.${UNARR_API_VERSION_MINOR} SOVERSION ${UNARR_API_VERSION_MAJOR})
