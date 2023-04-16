cmake_minimum_required(VERSION 3.0.0)
project(SoftRenderer)

include_directories(ThirdParty)
include_directories(Math)
include_directories(Render/Shader)
include_directories(Render)
include_directories(Custom)

if(WIN32)
    include_directories(Window/Win)
    set(SOURCES ${SOURCES} Window/Win/Window.cpp)
endif()

set(SOURCES ${SOURCES} Math/Mathf.cpp)
set(SOURCES ${SOURCES} Render/Shader/Shader.cpp)
set(SOURCES ${SOURCES} Render/Render.cpp)
set(SOURCES ${SOURCES} main.cpp)

add_executable(SoftRenderer ${SOURCES})