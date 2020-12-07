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

add_library(karchive-c-unarr OBJECT ${unarr_SRCS})
if (UNIX OR MINGW)
    target_compile_options(karchive-c-unarr PRIVATE -std=gnu99 -fomit-frame-pointer -D_FILE_OFFSET_BITS=64 -fPIC)
    set_property(TARGET karchive-c-unarr PROPERTY AUTOMOC OFF)
endif (UNIX OR MINGW)

if (ZLIB_FOUND)
    target_include_directories(karchive-c-unarr PRIVATE ${ZLIB_INCLUDE_DIRS})
endif(ZLIB_FOUND)
