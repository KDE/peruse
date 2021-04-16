set(APP_SRCS
    main.cpp
    resources.qrc
)

ecm_add_app_icon(APP_SRCS ICONS "icon/128-apps-peruse.png")

ecm_qt_declare_logging_category(APP_SRCS
    HEADER app_debug.h
    IDENTIFIER APP_LOG
    CATEGORY_NAME org.kde.peruse.app
    DEFAULT_SEVERITY Warning
)

add_executable(peruse ${APP_SRCS})
target_link_libraries(peruse
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
target_include_directories(peruse
    PRIVATE
    "${CMAKE_BINARY_DIR}"
)

install(TARGETS peruse ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES org.kde.peruse.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES org.kde.peruse.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

add_subdirectory(icon)
