set(qmlplugin_SRCS
    qmlplugin.cpp
    BookListModel.cpp
)

add_library (peruseqmlplugin SHARED ${qmlplugin_SRCS})

target_link_libraries (peruseqmlplugin
    Qt5::Qml
)

install (TARGETS peruseqmlplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/peruse)
install (FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/peruse)
