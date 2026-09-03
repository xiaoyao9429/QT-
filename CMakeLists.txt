cmake_minimum_required(VERSION 3.16)

project(Landlords VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# === 源文件按层分组 ===

# Domain 层：纯数据模型（Card / Cards / Player 及其派生类）
set(DOMAIN_SOURCES
    domain/card.h            domain/card.cpp
    domain/cards.h           domain/cards.cpp
    domain/player.h          domain/player.cpp
    domain/userplayer.h      domain/userplayer.cpp
    domain/robotplayer.h     domain/robotplayer.cpp
)

# App 层：游戏状态机与流程控制
set(APP_SOURCES
    app/gamecontrol.h        app/gamecontrol.cpp
)

# View 层：UI 控件与主窗口
set(VIEW_SOURCES
    view/gamemainwindow.h    view/gamemainwindow.cpp    view/gamemainwindow.ui
    view/cardpanel.h         view/cardpanel.cpp
    view/dealanimator.h      view/dealanimator.cpp
    view/buttongroup.h       view/buttongroup.cpp       view/buttongroup.ui
    view/scorepanel.h        view/scorepanel.cpp        view/scorepanel.ui
    view/mybutton.h          view/mybutton.cpp
)

set(UTILITY_SOURCES
    utility/playhand.h    utility/playhand.cpp
    utility/strategy.h    utility/strategy.cpp
)


set(TEST test/test.h)

set(PROJECT_SOURCES
    main.cpp
    ${DOMAIN_SOURCES}
    ${APP_SOURCES}
    ${VIEW_SOURCES}
    ${UTILITY_SOURCES}
    ${TEST}
    res.qrc
)

# 每层目录加入 include 路径，使 #include "xxx.h" 无需改动
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/domain
    ${PROJECT_SOURCE_DIR}/app
    ${PROJECT_SOURCE_DIR}/view
    ${PROJECT_SOURCE_DIR}/utility
    ${PROJECT_SOURCE_DIR}/test
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Landlords
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        view/animationwindow.h view/animationwindow.cpp view/animationwindow.ui

    )
else()
    if(ANDROID)
        add_library(Landlords SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(Landlords
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Landlords PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Landlords)
endif()
set_target_properties(Landlords PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Landlords
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Landlords)
endif()
