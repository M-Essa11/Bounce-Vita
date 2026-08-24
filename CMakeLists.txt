cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "VitaSDK toolchain")
  else()
    message(FATAL_ERROR "Define VITASDK before configuring this project.")
  endif()
endif()

project(bounce_zero_vita C)
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_APP_NAME "Bounce Vita")
set(VITA_TITLEID "BZERO0001")
set(VITA_VERSION "00.01")
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror=implicit-function-declaration")
add_compile_definitions(BOUNCE_VITA)

add_executable(${PROJECT_NAME}
  src/main_vita.c
  src/graphics_vita.c
  src/input_vita.c
  src/png_vita.c
  src/sound.c
  src/save_vita.c
  src/game.c
  src/physics.c
  src/level.c
  src/cbmf.c
  src/menu.c
  src/tile_table.c
  src/local.c
  src/local_extra.c
  src/splash.c
)

target_link_libraries(${PROJECT_NAME}
  vita2d
  png
  z
  m
  SceCtrl_stub
  SceAppMgr_stub
  SceAudio_stub
  SceCommonDialog_stub
  SceDisplay_stub
  SceGxm_stub
  SceKernelThreadMgr_stub
  SceProcessmgr_stub
  SceIofilemgr_stub
  SceSysmodule_stub
)

vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})
vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE assets/vita/icon0.png sce_sys/icon0.png
  FILE icons icons
  FILE levels levels
  FILE sounds sounds
  FILE lang lang
  FILE fonts fonts
)
