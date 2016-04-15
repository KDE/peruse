set(APP_SRCS
    main.cpp
)

ecm_add_app_icon(APP_SRCS ICONS "icon/128-apps-peruse.png")

add_executable(peruse ${APP_SRCS})
target_link_libraries(peruse
    Qt5::Core
    Qt5::Quick
    Qt5::Gui
    Qt5::Qml
    Qt5::Widgets
    Qt5::OpenGL
    KF5::I18n
)

install(TARGETS peruse ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES org.kde.peruse.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(DIRECTORY qml DESTINATION ${DATA_INSTALL_DIR}/peruse)

add_subdirectory(icon)
