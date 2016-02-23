project(karchive-rar)

# For unarr
add_subdirectory(external)

set(karchive_rar_SRCS
    KRar.cpp
    KRarFileEntry.cpp
)

add_library(karchive-rar STATIC ${karchive_rar_SRCS})
target_link_libraries(karchive-rar
    unarr
    KF5::Archive
)
