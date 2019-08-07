set(CREATOR_SRCS
    main.cpp
    resources.qrc
)

ecm_add_app_icon(CREATOR_SRCS ICONS "icon/128-apps-peruse-creator.png")

add_executable(perusecreator ${CREATOR_SRCS})
target_link_libraries(perusecreator
    Qt5::Core
    Qt5::Quick
    Qt5::Gui
    ${Qt5Gui_OPENGL_LIBRARIES}
    Qt5::Qml
    Qt5::Widgets
    Qt5::OpenGL
    KF5::I18n
    KF5::Declarative
    peruse_helpers
)

install(TARGETS perusecreator ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES org.kde.perusecreator.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES org.kde.perusecreator.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

add_subdirectory(icon)

