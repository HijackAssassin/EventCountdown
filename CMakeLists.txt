cmake_minimum_required(VERSION 3.16)

project(EventCountdowns VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    countdowntile.cpp
    countdowntile.h



)

# Add Windows icon resource if building on Windows
if(WIN32)
    list(APPEND PROJECT_SOURCES appicon.rc)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(EventCountdowns
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}


        utils.h

        utils.cpp
        resources.qrc
        editcountdowndialog.h
        editcountdowndialog.cpp
        outlinedlabel.h



    )
else()
    if(ANDROID)
        add_library(EventCountdowns SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(EventCountdowns
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(EventCountdowns PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.EventCountdowns)
endif()

set_target_properties(EventCountdowns PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS EventCountdowns
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
target_sources(EventCountdowns PRIVATE ${APP_RESOURCES})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(EventCountdowns)
endif()
