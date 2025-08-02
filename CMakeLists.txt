cmake_minimum_required(VERSION 3.16)
project(TetrisGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
  add_compile_options("/W4" "/permissive-")
else()
  add_compile_options(-Wall -Wextra -pedantic)
endif()

file(GLOB_RECURSE CORE_SOURCES       "${CMAKE_SOURCE_DIR}/src/core/*.cpp")
file(GLOB_RECURSE MODEL_SOURCES      "${CMAKE_SOURCE_DIR}/src/model/*.cpp")
file(GLOB_RECURSE CONTROLLER_SOURCES "${CMAKE_SOURCE_DIR}/src/controller/*.cpp")
file(GLOB_RECURSE ENGINE_SOURCES     "${CMAKE_SOURCE_DIR}/src/engine/*.cpp")
file(GLOB_RECURSE UI_SOURCES         "${CMAKE_SOURCE_DIR}/src/ui/*.cpp")
file(GLOB_RECURSE COMPONENT_SOURCES  "${CMAKE_SOURCE_DIR}/src/ui/components/*.cpp")
file(GLOB_RECURSE APP_SOURCES        "${CMAKE_SOURCE_DIR}/src/app/*.cpp")

set(ALL_SOURCES
  ${CORE_SOURCES}
  ${MODEL_SOURCES}
  ${CONTROLLER_SOURCES}
  ${ENGINE_SOURCES}
  ${UI_SOURCES}
  ${COMPONENT_SOURCES}
  ${APP_SOURCES}
  "${CMAKE_SOURCE_DIR}/src/engine/resources/Resources.rc" 
)

add_definitions(-DUNICODE -D_UNICODE)

add_executable(${PROJECT_NAME} WIN32 ${ALL_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
  "${CMAKE_SOURCE_DIR}/src"
  "${CMAKE_SOURCE_DIR}/src/engine/resources"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  d2d1
  Dcomp
  dwrite
  d3d11
  dxgi
  Windowscodecs
  winmm
  Shlwapi
  Shcore
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/Debug"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
)