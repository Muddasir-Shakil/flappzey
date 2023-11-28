# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
# set(ZEPHYR_TOOLCHAIN_VARIANT gnuarmemb)
#set(BOARD_FLASH_RUNNER jlink)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(blinky)

zephyr_include_directories(src)

target_sources(app PRIVATE src/main.c src/flappy.c src/flappy_lvgl.c )
