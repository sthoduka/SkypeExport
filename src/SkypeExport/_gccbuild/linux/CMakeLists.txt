cmake_minimum_required(VERSION 2.8)

project(SkypeExport)

find_package(Boost 1.46 COMPONENTS
    program_options
    filesystem
    system
    regex
    REQUIRED
)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebKitWidgets)
set(qt_UI
  "../../view/skype_export.ui"
)
QT5_WRAP_UI(qt_UI_H ${qt_UI})

set(qt_H
  "release/ui_skype_export.h")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE AUTOUIC ON)
QT5_WRAP_CPP(qt_H_MOC ${qt_H})
QT5_WRAP_UI(qt_UI_H ${qt_UI})


add_executable(SkypeExport
    ../../main.cpp
    ../../libs/sqlite3/sqlite3.c
    ../../model/skypeparser_core.cpp
    ../../model/skypeparser_parsing.cpp
    ../../model/skypeparser_utils.cpp
    ../../view/skype_export_window.cpp
    ../../view/skype_export_window.h
    ${qt_UI_H}
)

include_directories(SkypeExport
    ${Boost_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5WebKitWidgets_INCLUDE_DIRS}
)

target_link_libraries(SkypeExport
    ${Boost_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5WebKitWidgets_LIBRARIES}
)

# disable sqlite3 threading and dynamic extension loader to avoid pthreads/dl dependencies
add_definitions(-DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION)
