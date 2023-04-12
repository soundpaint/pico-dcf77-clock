cmake_minimum_required(VERSION 3.18)

# pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)

# we also need PICO EXTRAS
include(pico_extras_import.cmake)

project(pico-dcf77-clock)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_CXX_ENABLE_EXCEPTIONS 1)
add_compile_options(-Wall -Wextra -Wnull-dereference)

# initialize the Pico SDK
pico_sdk_init()

add_executable(pico-dcf77-clock
  src/dcf77-clock.cpp
  src/dcf77.cpp
  src/gui.cpp
  src/hd44780.cpp
  src/hw061.cpp)

# pull in common dependencies
target_link_libraries(pico-dcf77-clock
  hardware_i2c
  hardware_rtc
  pico_stdlib)

# enable serial output via USB
pico_enable_stdio_usb(pico-dcf77-clock 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(pico-dcf77-clock)

# Local variables:
#   mode: cmake
#   coding: utf-8
# End:
