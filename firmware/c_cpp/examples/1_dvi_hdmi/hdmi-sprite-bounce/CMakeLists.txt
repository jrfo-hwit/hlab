# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(hdmi C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(hdmi hdmi.c )

pico_set_program_name(hdmi "hdmi")
pico_set_program_version(hdmi "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(hdmi 0)
pico_enable_stdio_usb(hdmi 0)

# Add the standard library to the build
target_link_libraries(hdmi
        pico_stdlib)


target_compile_definitions(hdmi PRIVATE
	DVI_DEFAULT_SERIAL_CONFIG=${DVI_DEFAULT_SERIAL_CONFIG}
	)

# Add the standard include files to the build
#target_include_directories(hdmi PRIVATE
#        ${CMAKE_CURRENT_LIST_DIR}
#)
add_subdirectory(libdvi)
add_subdirectory(libsprite)

# Add any user requested libraries
target_link_libraries(hdmi
	pico_stdlib
	pico_multicore
	pico_util
	libdvi
	libsprite
)

pico_add_extra_outputs(hdmi)
