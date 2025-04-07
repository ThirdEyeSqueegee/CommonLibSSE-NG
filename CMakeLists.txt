cmake_minimum_required(VERSION 3.30)

option(REX_OPTION_INI "Enables ini config support for REX." OFF)
option(REX_OPTION_JSON "Enables json config support for REX." OFF)
option(REX_OPTION_TOML "Enables toml config support for REX." OFF)
option(SKSE_SUPPORT_XBYAK "Enables trampoline support for xbyak." OFF)
option(ENABLE_SKYRIM_SE "Enable support for Skyrim SE." ON)
option(ENABLE_SKYRIM_AE "Enable support for Skyrim AE." ON)
option(ENABLE_SKYRIM_VR "Enable support for Skyrim VR." ON)

message("Options:")
message("  Enable xbyak: ${SKSE_SUPPORT_XBYAK}")
message("  Enable Skyrim SE: ${ENABLE_SKYRIM_SE}")
message("  Enable Skyrim AE: ${ENABLE_SKYRIM_AE}")
message("  Enable Skyrim VR: ${ENABLE_SKYRIM_VR}")

if(NOT ENABLE_SKYRIM_SE AND NOT ENABLE_SKYRIM_AE AND NOT ENABLE_SKYRIM_VR)
  message(FATAL_ERROR "At least one Skyrim runtime must be supported by the CommonLibSSE build.")
endif()

project(
  CommonLibSSE
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OPTIMIZE_DEPENDENCIES ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)

if(${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
  message(FATAL_ERROR "In-source builds are not allowed")
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(spdlog CONFIG REQUIRED)
find_package(directxtk CONFIG REQUIRED)

if(ENABLE_SKYRIM_VR)
  find_package(OpenVR REQUIRED)
  find_path(RAPIDCSV_INCLUDE_DIRS rapidcsv.h)
endif()

file(
  GLOB_RECURSE
  SOURCES
  CONFIGURE_DEPENDS
   ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/include/SKSE/Impl/PCH.h)

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
	"$<$<BOOL:${REX_OPTION_INI}>:REX_OPTION_INI=1>"
	"$<$<BOOL:${REX_OPTION_JSON}>:REX_OPTION_JSON=1>"
	"$<$<BOOL:${REX_OPTION_TOML}>:REX_OPTION_TOML=1>"
  $<$<BOOL:${SKSE_SUPPORT_XBYAK}>:SKSE_SUPPORT_XBYAK>
  $<$<BOOL:${ENABLE_SKYRIM_SE}>:ENABLE_SKYRIM_SE>
  $<$<BOOL:${ENABLE_SKYRIM_AE}>:ENABLE_SKYRIM_AE>
  $<$<BOOL:${ENABLE_SKYRIM_VR}>:ENABLE_SKYRIM_VR>
  HAS_SKYRIM_MULTI_TARGETING
  WIN32_LEAN_AND_MEAN
  NOMINMAX
)

target_compile_options(
  ${PROJECT_NAME}
  PRIVATE
  /wd4005 # macro redefinition
  /wd4068 # unknown pragma
  /wd4200 # nonstandard extension used: zero-sized array in struct/union
  /wd4201 # nonstandard extension used: nameless struct/union
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
  $<$<BOOL:${ENABLE_SKYRIM_VR}>:${RAPIDCSV_INCLUDE_DIRS}>
)

target_precompile_headers(
  ${PROJECT_NAME}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/SKSE/Impl/PCH.h
)

target_compile_options(
  ${PROJECT_NAME}
  PRIVATE
  /cgthreads8 /diagnostics:caret /jumptablerdata /MP /W4 /Zc:__cplusplus /Zc:enumTypes /Zc:inline /Zc:templateScope
)

if(CMAKE_BUILD_TYPE STREQUAL Release)
  target_compile_options(
    ${PROJECT_NAME}
    PRIVATE
    /fp:fast /Ob3 /GL /GR- /Gw /Qpar
  )
  target_link_options(
    ${PROJECT_NAME}
    PRIVATE
    /CGTHREADS:8 /INCREMENTAL:NO /OPT:ICF=4
  )
endif()

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC
  spdlog::spdlog
  Microsoft::DirectXTK
  $<$<BOOL:${ENABLE_SKYRIM_VR}>:OpenVR::OpenVR>
  Advapi32.lib
	bcrypt.lib
  D3D11.lib
  d3dcompiler.lib
  Dbghelp.lib
  DXGI.lib
  Ole32.lib
  Version.lib
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
  include/RE
  include/REL
  include/REX
  include/SKSE
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
