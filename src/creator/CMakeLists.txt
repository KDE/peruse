set(CREATOR_SRCS
    main.cpp
)

# ecm_add_app_icon(CREATOR_SRCS ICONS "icon/128-apps-peruse.png")

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
)

install(TARGETS perusecreator ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES org.kde.perusecreator.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(DIRECTORY qml DESTINATION ${DATA_INSTALL_DIR}/perusecreator)
install(FILES org.kde.perusecreator.appdata.xml DESTINATION ${DATA_INSTALL_DIR}/appdata)

# add_subdirectory(icon)
