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
target_include_directories(peruse
    PRIVATE
    "${CMAKE_BINARY_DIR}"
)

install(TARGETS peruse ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES org.kde.peruse.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.peruse.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

add_subdirectory(icon)
