cmake_minimum_required(VERSION 2.8.12)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(walk)
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q -Wall -O3 -nostdlib")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions")

execute_process(
  COMMAND git describe --always
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_VERSION
)
string(REGEX REPLACE "\n$" "" GIT_VERSION "${GIT_VERSION}")

configure_file(
  "${CMAKE_SOURCE_DIR}/walk_config.h.in"
  "${CMAKE_BINARY_DIR}/walk_config.h"
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROJECT_NAME}
  draw.c
  main.c
)

target_link_libraries(${PROJECT_NAME}
  gcc
  taihen_stub
  taihenModuleUtils_stub
  SceAppMgr_stub
  SceDisplay_stub
  SceCtrl_stub
  SceIofilemgr_stub
  SceLibc_stub
  SceLibKernel_stub
)

vita_create_self(${PROJECT_NAME}.suprx ${PROJECT_NAME} CONFIG exports.yml UNSAFE)

add_custom_target(send
  COMMAND curl -T ${PROJECT_NAME}.suprx ftp://$(PSVITAIP):1337/ux0:/tai/
  DEPENDS ${PROJECT_NAME}.suprx
)