cmake_minimum_required(VERSION 3.16)

project(JukeBox VERSION 0.1 LANGUAGES CXX)  # اسم پروژه رو من روی JukeBox گذاشتم، چون احتمالا این اصلیه

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Network Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Network Multimedia)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        hostwindow.cpp
        hostwindow.h
        hostwindow.ui
        server.h server.cpp
        client.h client.cpp
        message_displayer.h message_displayer.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(JukeBox
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        joinwindow.h joinwindow.cpp joinwindow.ui
        server.h server.cpp
        client.h client.cpp
        message_displayer.h message_displayer.cpp
        user.h
        user.cpp
        commandtype.h
        command.h command.cpp
        musicroom.h musicroom.cpp musicroom.ui
        musicroom.h musicroom.cpp musicroom.ui
        authmanager.h
        authmanager.cpp
        resources.qrc
    )
else()
    if(ANDROID)
        add_library(JukeBox SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(JukeBox
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(JukeBox PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Multimedia)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.JukeBox)
endif()
set_target_properties(JukeBox PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS JukeBox
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(JukeBox)
endif()
