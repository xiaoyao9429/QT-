cmake_minimum_required(VERSION 3.16)

project(Landlords VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)

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
        endpanel.h endpanel.cpp
        view/countdown.h view/countdown.cpp
        app/bgmcontroller.h app/bgmcontroller.cpp
        view/loading.h view/loading.cpp

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

if(WIN32)
    # exe 文件图标（资源管理器显示），MinGW 下 CMake 自动用 windres 编译 .rc
    target_sources(Landlords PRIVATE app.rc)
endif()

target_link_libraries(Landlords PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia)

# ===== 构建前自动归一化源文件 UTF-8 BOM（防止重复 BOM 导致 stray '#' 编译错误）=====
# 自定义 target 每次构建先执行：扫描所有 .h/.cpp，保证开头恰好一个 BOM
# 脚本只重写需要修复的文件，BOM 已正确的文件不动时间戳，不会触发无意义重编译
if(WIN32)
    set(POWERSHELL_EXE powershell)
    add_custom_target(normalize_bom ALL
        COMMAND ${POWERSHELL_EXE} -NoProfile -ExecutionPolicy Bypass
                -File "${CMAKE_SOURCE_DIR}/scripts/normalize_bom.ps1"
                -Root "${CMAKE_SOURCE_DIR}"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Normalizing UTF-8 BOM in source files"
        VERBATIM
    )
    add_dependencies(Landlords normalize_bom)
endif()

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
