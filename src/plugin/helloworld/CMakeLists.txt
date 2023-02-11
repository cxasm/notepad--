cmake_minimum_required(VERSION 3.16)
project(helloworld)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Concurrent Network PrintSupport XmlPatterns)

add_definitions(-D_UNICODE -DUNICODE)




file(GLOB UI_SRC ${PROJECT_SOURCE_DIR}/*.ui)
file(GLOB SRC ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB MOC_HEADER ${PROJECT_SOURCE_DIR}/*.h)
# add_executable(${PROJECT_NAME} ${IS_WIN} ${SRC} ${UI_SRC} ${PROJECT_SOURCE_DIR}/src/RealCompare.qrc)

add_library(${PROJECT_NAME} SHARED ${SRC} ${UI_SRC} ${MOC_HEADER})

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

target_link_libraries(${PROJECT_NAME} qscint Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent Qt5::Network  Qt5::PrintSupport Qt5::XmlPatterns)

# if(NOT DEFINED ${notepad--_BINARY_DIR})
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${notepad--_BINARY_DIR}/bin/plugin)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${notepad--_BINARY_DIR}/bin/plugin)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${notepad--_BINARY_DIR}/bin/plugin)
# set(LIBRARY_OUTPUT_PATH ${notepad--_BINARY_DIR}/bin/plugin)
# set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${notepad--_BINARY_DIR}/bin/plugin)
# endif()
