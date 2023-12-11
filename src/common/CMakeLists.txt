add_library(peruse_helpers STATIC peruse_helpers.cpp )
target_link_libraries(peruse_helpers
    KF6::CoreAddons
    KF6::Crash
    KF6::IconThemes
    KF6::I18n
    Qt6::OpenGL
    Qt6::Quick
)

if (NOT ANDROID)
    target_link_libraries(peruse_helpers Qt6::Widgets)
endif()
