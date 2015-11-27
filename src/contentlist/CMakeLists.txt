set(qmlplugin_SRCS
    qmlplugin.cpp
    ContentList.cpp
    BalooContentLister.cpp
)

add_library (contentlistqmlplugin SHARED ${qmlplugin_SRCS})

target_link_libraries (contentlistqmlplugin
    Qt5::Qml
    KF5::Baloo
)

install (TARGETS contentlistqmlplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/contentlist)
install (FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/contentlist)
