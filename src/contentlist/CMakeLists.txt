set(qmlplugin_SRCS
    qmlplugin.cpp
    ContentList.cpp
    ContentListerBase.cpp
    FilesystemContentLister.cpp
    ContentQuery.cpp
)


if(KF5Baloo_FOUND)
    set(qmlplugin_baloo_SRCS
        BalooContentLister.cpp
    )
endif()

add_library (contentlistqmlplugin SHARED ${qmlplugin_SRCS} ${qmlplugin_baloo_SRCS})

target_link_libraries (contentlistqmlplugin
    Qt5::Gui
    Qt5::Qml
    KF5::FileMetaData
)

if(KF5Baloo_FOUND)
    target_compile_definitions(contentlistqmlplugin PRIVATE -DBALOO_FOUND="${KF5Baloo_FOUND}")
    target_link_libraries(contentlistqmlplugin    
        KF5::Baloo
    )
endif()

install (TARGETS contentlistqmlplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/contentlist)
install (FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/contentlist)
