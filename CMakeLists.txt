cmake_minimum_required(VERSION 3.30)

option(SKSE_SUPPORT_XBYAK "Enables trampoline support for Xbyak." OFF)
option(ENABLE_SKYRIM_SE "Enable support for Skyrim SE in the dynamic runtime feature." ON)
option(ENABLE_SKYRIM_AE "Enable support for Skyrim AE in the dynamic runtime feature." ON)
option(ENABLE_SKYRIM_VR "Enable support for Skyrim VR in the dynamic runtime feature." ON)

message("Options:")
message("\tEnable Xbyak: ${SKSE_SUPPORT_XBYAK}")
message("\tEnable Skyrim SE: ${ENABLE_SKYRIM_SE}")
message("\tEnable Skyrim AE: ${ENABLE_SKYRIM_AE}")
message("\tEnable Skyrim VR: ${ENABLE_SKYRIM_VR}")

if(NOT ENABLE_SKYRIM_SE AND NOT ENABLE_SKYRIM_AE AND NOT ENABLE_SKYRIM_VR)
  message(FATAL_ERROR "At least one Skyrim runtime must be supported by the CommonLibSSE build.")
endif()

project(
  CommonLibSSE
  LANGUAGES CXX
  VERSION 3.6.0
)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF)
set(CMAKE_OPTIMIZE_DEPENDENCIES ON)

include(GNUInstallDirs)

if(${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
  message(FATAL_ERROR "in-source builds are not allowed")
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(spdlog CONFIG REQUIRED)
find_path(RAPIDCSV_INCLUDE_DIRS "rapidcsv.h")
find_package(directxtk CONFIG REQUIRED)

if(ENABLE_SKYRIM_VR)
  find_package(OpenVR REQUIRED)
endif()

file(GLOB_RECURSE SOURCES include/**/*.h src/**/*.cpp)
list(REMOVE_ITEM headers ${CMAKE_CURRENT_SOURCE_DIR}/include/SKSE/Impl/PCH.h)

source_group(
  TREE ${CMAKE_CURRENT_SOURCE_DIR}
  FILES ${SOURCES}
)

add_library(
  ${PROJECT_NAME}
  STATIC
  ${SOURCES}
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_definitions(
  ${PROJECT_NAME}
  PUBLIC
  WINVER=0x0601 # windows 7, minimum supported version by skyrim special edition
  _WIN32_WINNT=0x0601
  "$<$<BOOL:${SKSE_SUPPORT_XBYAK}>:SKSE_SUPPORT_XBYAK=1>"
  "$<$<BOOL:${ENABLE_SKYRIM_SE}>:ENABLE_SKYRIM_SE=1>"
  "$<$<BOOL:${ENABLE_SKYRIM_AE}>:ENABLE_SKYRIM_AE=1>"
  "$<$<BOOL:${ENABLE_SKYRIM_VR}>:ENABLE_SKYRIM_VR=1>"
  HAS_SKYRIM_MULTI_TARGETING=1
)

target_compile_options(
  ${PROJECT_NAME}
  PUBLIC
  /wd4005 # macro redefinition
  /wd4068 # unknown pragma
  /wd4200 # nonstandard extension used: zero-sized array in struct/union
  /wd4201 # nonstandard extension used: nameless struct/union
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
  ${spdlog_INCLUDE_DIRS}
  ${RAPIDCSV_INCLUDE_DIRS}
)

target_compile_definitions(
  ${PROJECT_NAME}
  PUBLIC
)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC
  spdlog::spdlog
  Microsoft::DirectXTK
  "$<$<BOOL:${ENABLE_SKYRIM_VR}>:OpenVR::OpenVR>"
  Advapi32.lib
  D3D11.lib
  d3dcompiler.lib
  Dbghelp.lib
  DXGI.lib
  Ole32.lib
  Version.lib
)

target_precompile_headers(
  ${PROJECT_NAME}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SKSE/Impl/PCH.h
)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}-targets
)

install(
  EXPORT ${PROJECT_NAME}-targets
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

configure_file(
  cmake/config.cmake.in
  ${PROJECT_NAME}Config.cmake
  @ONLY
)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
  DIRECTORY
  "include/RE"
  "include/REL"
  "include/REX"
  "include/SKSE"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
