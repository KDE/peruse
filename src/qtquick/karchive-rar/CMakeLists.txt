project(karchive-rar)

find_package(ZLIB)

set(unarr_SRCS
    unarr/rar/uncompress-rar.c
    unarr/rar/huffman-rar.c
    unarr/rar/rar.c
    unarr/rar/filter-rar.c
    unarr/rar/rarvm.c
    unarr/rar/parse-rar.c
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

set(karchive_rar_SRCS
    KRar.cpp
    KRarFileEntry.cpp
)

add_library(karchive-c-unarr OBJECT ${unarr_SRCS})
if (UNIX OR MINGW)
    target_compile_options(karchive-c-unarr PRIVATE -std=gnu99 -fomit-frame-pointer -D_FILE_OFFSET_BITS=64 -fPIC)
    set_property(TARGET karchive-c-unarr PROPERTY AUTOMOC OFF)
endif (UNIX OR MINGW)


add_library(karchive-rar STATIC ${karchive_rar_SRCS} $<TARGET_OBJECTS:karchive-c-unarr>)
target_link_libraries(karchive-rar
    PUBLIC KF5::Archive
)
target_include_directories(karchive-rar
    PRIVATE
    unarr/
)

if (ZLIB_FOUND)
    target_include_directories(karchive-c-unarr PRIVATE ${ZLIB_INCLUDE_DIRS})
    target_include_directories(karchive-rar PRIVATE ${ZLIB_INCLUDE_DIRS})
    target_link_libraries(karchive-rar PRIVATE ${ZLIB_LIBRARIES})
    add_definitions(-DHAVE_ZLIB)
endif(ZLIB_FOUND)

# A little hack, which makes the karchive_rar library think it's a part of KF5Archive
add_definitions(-DKF5Archive_EXPORTS)
