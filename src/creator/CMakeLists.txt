set(CREATOR_SRCS
    main.cpp
    resources.qrc
)

ecm_add_app_icon(CREATOR_SRCS ICONS "icon/128-apps-peruse-creator.png")

add_executable(perusecreator ${CREATOR_SRCS})
target_link_libraries(perusecreator
    Qt::Core
    Qt::Quick
    Qt::Gui
    ${QtGui_OPENGL_LIBRARIES}
    Qt::Qml
    Qt::Widgets
    Qt::OpenGL
    KF${QT_MAJOR_VERSION}::I18n
    peruse_helpers
)
target_include_directories(perusecreator
    PRIVATE
    "${CMAKE_BINARY_DIR}"
)

install(TARGETS perusecreator ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES org.kde.perusecreator.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.perusecreator.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

add_subdirectory(icon)

