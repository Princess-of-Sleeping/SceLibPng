cmake_minimum_required(VERSION 3.0)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(SceLibPng)
include("${VITASDK}/share/vita.cmake" REQUIRED)
include(ExternalProject)

set(CMAKE_C_FLAGS "-Wl,-q -Wall -Os -nostdlib -fno-inline")

# set(VITA_ELF_CREATE_FLAGS "${VITA_ELF_CREATE_FLAGS} -vvv")
# set(VITA_MAKE_FSELF_FLAGS "${VITA_MAKE_FSELF_FLAGS} -a 0x2808000000000000")

enable_language(C ASM)

ExternalProject_add(libpng_download
  GIT_REPOSITORY git://git.code.sf.net/p/libpng/code#tag=v1.6.40
  #GIT_TAG "1.6.40"
  DOWNLOAD_DIR ""
  PATCH_COMMAND patch -d <SOURCE_DIR> -p3 -t -N < ${PROJECT_SOURCE_DIR}/libpng.patch
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)

set(LIBPNG_SRC
  ${CMAKE_CURRENT_BINARY_DIR}/libpng_download-prefix/src/libpng_download/png.c
  ${CMAKE_CURRENT_BINARY_DIR}/libpng_download-prefix/src/libpng_download/pngerror.c
  ${CMAKE_CURRENT_BINARY_DIR}/libpng_download-prefix/src/libpng_download/pngget.c
  ${CMAKE_CURRENT_BINARY_DIR}/libpng_download-prefix/src/libpng_download/pngmem.c
  ${CMAKE_CURRENT_BINARY_DIR}/libpng_download-prefix/src/libpng_download/pngpread.c
  ${CMAKE_CURRENT_BINARY_DIR}/libpng_download-prefix/src/libpng_download/pngread.c
  ${CMAKE_CURRENT_BINARY_DIR}/libpng_download-prefix/src/libpng_download/pngrio.c
  ${CMAKE_CURRENT_BINARY_DIR}/libpng_download-prefix/src/libpng_download/pngrtran.c
  ${CMAKE_CURRENT_BINARY_DIR}/libpng_download-prefix/src/libpng_download/pngrutil.c
  ${CMAKE_CURRENT_BINARY_DIR}/libpng_download-prefix/src/libpng_download/pngset.c
  ${CMAKE_CURRENT_BINARY_DIR}/libpng_download-prefix/src/libpng_download/pngtrans.c
  ${CMAKE_CURRENT_BINARY_DIR}/libpng_download-prefix/src/libpng_download/pngwio.c
  ${CMAKE_CURRENT_BINARY_DIR}/libpng_download-prefix/src/libpng_download/pngwrite.c
  ${CMAKE_CURRENT_BINARY_DIR}/libpng_download-prefix/src/libpng_download/pngwtran.c
  ${CMAKE_CURRENT_BINARY_DIR}/libpng_download-prefix/src/libpng_download/pngwutil.c
  ${CMAKE_CURRENT_BINARY_DIR}/libpng_download-prefix/src/libpng_download/arm/arm_init.c
  ${CMAKE_CURRENT_BINARY_DIR}/libpng_download-prefix/src/libpng_download/arm/filter_neon.S
)

add_custom_command(
  OUTPUT ${LIBPNG_SRC}
  DEPENDS ${libpng_download}
)

link_directories(
  # ../kernel/build/stubs/
)

add_executable(${PROJECT_NAME}
  src/main.c
  ${LIBPNG_SRC}
)

target_link_libraries(${PROJECT_NAME}
  SceZlib_stub
  SceLibc_stub
  SceRtabi_stub
)

set_target_properties(${PROJECT_NAME}
  PROPERTIES LINK_FLAGS "-nostdlib"
  COMPILE_FLAGS "-D__PSP2_USER__ -DPNG_WEIGHT_SHIFT=8 -DPNG_COST_SHIFT=3 -DPNG_ERROR_NUMBERS_SUPPORTED -DPNG_ARM_NEON_IMPLEMENTATION=2 -DPNG_ARM_NEON_OPT=1"
)

vita_create_self(libpng.suprx ${PROJECT_NAME}
  CONFIG exports.yml
  UNSAFE
  STRIPPED
  REL_OPTIMIZE
)

vita_create_stubs(stubs_${PROJECT_NAME} ${PROJECT_NAME} exports.yml)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libSceLibPng_stub.a ${CMAKE_CURRENT_BINARY_DIR}/libSceLibPng_stub_weak.a
  DESTINATION lib
)
