add_library(peruse_helpers STATIC peruse_helpers.cpp )
target_link_libraries(peruse_helpers
    KF5::Crash
    KF5::Declarative
    KF5::I18n
    Qt5::OpenGL
)
