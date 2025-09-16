cmake_minimum_required(VERSION 3.19)
project(QAnimatedGridLayout_Test LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(QAnimatedGridLayout_Test
    sources/main.cpp
    headers/qanimatedgridlayout.h
    headers/qflowlayout.h
    sources/qanimatedgridlayout.cpp
    sources/qflowlayout.cpp
)

target_include_directories(QAnimatedGridLayout_Test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/headers
)


target_link_libraries(QAnimatedGridLayout_Test
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS QAnimatedGridLayout_Test
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
