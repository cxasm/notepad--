cmake_minimum_required(VERSION 3.16)
project(NotePad-- VERSION 1.22.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 项目根目录（CMakeLists.txt 在 how_build/ 子目录下）
set(PROJECT_ROOT ${PROJECT_SOURCE_DIR}/..)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Concurrent Network PrintSupport XmlPatterns)

# qscint 关键依赖库
add_subdirectory(${PROJECT_ROOT}/src/qscint ${PROJECT_BINARY_DIR}/qscint)

# 插件库包含
# helloworld 动态插件库
add_subdirectory(${PROJECT_ROOT}/src/plugin/helloworld ${PROJECT_BINARY_DIR}/helloworld)
# xmledit XML工具插件库
add_subdirectory(${PROJECT_ROOT}/src/plugin/xmledit ${PROJECT_BINARY_DIR}/xmledit)

# win下需要开启UNICODE进行支持TCHAR
if(CMAKE_HOST_WIN32)
    add_definitions(-D_UNICODE -DUNICODE)
endif()


if(${PLUGIN_EN})
    if(${PLUGIN_EN}  STREQUAL on)
        add_definitions(-DNO_PLUGIN=1)
    endif(${PLUGIN_EN})
endif()



file(GLOB UI_SRC ${PROJECT_ROOT}/src/*.ui)
set(UI_SRC ${UI_SRC} ${PROJECT_ROOT}/src/cceditor/ccnotepad.ui)
aux_source_directory(${PROJECT_ROOT}/src SRC)
aux_source_directory(${PROJECT_ROOT}/src/cceditor SRC)


if(CMAKE_HOST_WIN32)
# 添加 WIN32 保证主程序启动没有命令行
    list(APPEND WIN_RCS ${PROJECT_ROOT}/src/RealCompareToMinGw.rc)
    add_executable(${PROJECT_NAME} WIN32 ${WIN_RCS} ${SRC} ${UI_SRC} ${PROJECT_ROOT}/src/RealCompare.qrc)
else()
    add_executable(${PROJECT_NAME} ${SRC} ${UI_SRC} ${PROJECT_ROOT}/src/RealCompare.qrc)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
${PROJECT_ROOT}/src
${PROJECT_ROOT}/src/cceditor

${PROJECT_ROOT}/src/qscint/src
${PROJECT_ROOT}/src/qscint/src/Qsci
${PROJECT_ROOT}/src/qscint/scintilla/src
${PROJECT_ROOT}/src/qscint/scintilla/include
${PROJECT_ROOT}/src/qscint/scintilla/lexlib
${PROJECT_ROOT}/src/qscint/scintilla/boostregex
)

target_link_libraries(${PROJECT_NAME} qscint Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent Qt5::Network  Qt5::PrintSupport Qt5::XmlPatterns)

# set(PROJECT_BINARY_DIR "${PROJECT_BINARY_DIR}/bin")
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

if(CMAKE_HOST_UNIX)
    install(
        TARGETS ${PROJECT_NAME}
        DESTINATION "bin"
    )

    install(TARGETS helloworld xmledit
            DESTINATION "bin/plugin")

    install(DIRECTORY  ${PROJECT_ROOT}/src/linux/usr
            DESTINATION "/")

    include(${PROJECT_SOURCE_DIR}/cmake/deb_package_config.cmake)
    include(CPack)
elseif(CMAKE_HOST_WIN32)
    install(TARGETS ${PROJECT_NAME}
            DESTINATION ".")

    install(TARGETS helloworld xmledit
            DESTINATION "plugin")
    # 设置软件版本
    set(CPACK_PACKAGE_NAME "NotePad--")
    set(CPACK_PACKAGE_DESCRIPTION "NotePad--")
    set(CPACK_PACKAGE_COPYRIGHT "Copyright (c) 2023")
    set(CPACK_PACKAGE_VERSION "1.22.0")
    set(CPACK_PACKAGE_VERSION_MAJOR "1")
    set(CPACK_PACKAGE_VERSION_MINOR "22")
    set(CPACK_PACKAGE_VERSION_PATCH "0")

    include(${PROJECT_SOURCE_DIR}/cmake/nsis_package_config.cmake) 
    include(CPack)
endif()
