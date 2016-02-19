project(karchive-rar)

set(karchive_rar_SRCS
    KRar.cpp
    KRarFileEntry.cpp
)

add_library(karchive-rar STATIC ${karchive_rar_SRCS})
target_link_libraries(karchive-rar
    archive
    KF5::Archive
)
