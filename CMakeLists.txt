cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME MecrispCube)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    Forth/mecrisp.s

    peripherals/block.c
    peripherals/bsp.c
    peripherals/button.c
    peripherals/clock.c
    peripherals/dcc.c
    peripherals/epd.c
    peripherals/fd.c
    peripherals/flash.c
    peripherals/font6x8.c
    peripherals/font8x8.c
    peripherals/font8x14.c
    peripherals/font12x16.c
    peripherals/fpu.c
    peripherals/fs.c
    peripherals/iic.c
    peripherals/log.c
    peripherals/mip.c
    peripherals/myassert.c
    peripherals/oled.c
    peripherals/plex.c
    peripherals/power.c
    peripherals/quad.c
    peripherals/rt_spi.c
    peripherals/rtos.c
    peripherals/sd.c
    peripherals/uart.c
    peripherals/usb_cdc.c
    peripherals/watchdog.c

    vi/vi.c

    tinyusb/msc_disk.c
    tinyusb/tiny.c
    tinyusb/tusb.c
    tinyusb/usb_descriptors.c
    tinyusb/bsp/board.c
    tinyusb/bsp/family.c
    tinyusb/class/cdc/cdc_device.c
    tinyusb/class/hid/hid_device.c
    tinyusb/class/msc/msc_device.c
    tinyusb/common/tusb_fifo.c
    tinyusb/device/usbd.c
    tinyusb/device/usbd_control.c
    tinyusb/portable/st/stm32_fsdev/dcd_stm32_fsdev.c
    tinyusb/portable/st/typec/typec_stm32.c
    tinyusb/typec/usbc.c

    STM32_WPAN/App/crs_app.c

    Middlewares/Third_Party/FatFs/src/option/ccsbcs.c
    Middlewares/ST/STM32_WPAN/ble/svc/Src/crs_stm.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    Forth
    Forth/common
    Forth/cube
    peripherals
    vi
    tinyusb
    tinyusb/bsp
    tinyusb/class
    tinyusb/class/cdc
    tinyusb/class/hid
    tinyusb/class/msc
    tinyusb/common
    tinyusb/device
    tinyusb/osal
    tinyusb/portable
    tinyusb/portable/st
    tinyusb/portable/st/stm32_fsdev
    tinyusb/portable/st/typec
    tinyusb/typec

)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
