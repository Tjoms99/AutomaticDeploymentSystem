# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(peripheral)

target_sources(app PRIVATE
  src/rs485/uart.c 
  src/rs485/rs485.c 
  src/main.c
  src/gpio/leds.c
  src/gpio/battery.c
  src/bluetooth/bluetooth.c
)
