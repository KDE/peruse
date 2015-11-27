set(APP_SRCS
    main.cpp
)

add_executable(peruse ${APP_SRCS})
target_link_libraries(peruse
    Qt5::Core
    Qt5::Quick
    Qt5::Gui
    Qt5::Qml
    Qt5::Widgets
    KF5::DBusAddons
    KF5::I18n
)

install(TARGETS peruse ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES org.kde.peruse.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(DIRECTORY qml DESTINATION ${DATA_INSTALL_DIR}/peruse)
