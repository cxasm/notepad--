cmake_minimum_required(VERSION 3.16)
project(xmledit)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Concurrent Network PrintSupport XmlPatterns)

add_definitions(-D_UNICODE -DUNICODE)

file(GLOB SRC ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB MOC_HEADER ${PROJECT_SOURCE_DIR}/*.h)

add_library(${PROJECT_NAME} SHARED ${SRC} ${MOC_HEADER})

# MSVC: 源文件为 UTF-8 编码，需要指定 /utf-8 避免 GBK 误解析
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/../../include
    ${PROJECT_SOURCE_DIR}/../../qscint/src
    ${PROJECT_SOURCE_DIR}/../../qscint/src/Qsci
    ${PROJECT_SOURCE_DIR}/../../qscint/scintilla/src
    ${PROJECT_SOURCE_DIR}/../../qscint/scintilla/include
    ${PROJECT_SOURCE_DIR}/../../qscint/scintilla/lexlib
    ${PROJECT_SOURCE_DIR}/../../qscint/scintilla/boostregex
)

target_link_libraries(${PROJECT_NAME}
    qscint
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Concurrent
    Qt5::Network
    Qt5::PrintSupport
    Qt5::XmlPatterns
)
