cmake_minimum_required(VERSION 3.19)
project(SpiderGame LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(SpiderGame
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    escenario.h escenario.cpp
    escenanivel1.h escenanivel1.cpp
    escenanivel2.h escenanivel2.cpp
    escena.h escena.cpp
)
add_custom_command(TARGET SpiderGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:SpiderGame>/assets"
)

target_link_libraries(SpiderGame
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS SpiderGame
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET SpiderGame
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
