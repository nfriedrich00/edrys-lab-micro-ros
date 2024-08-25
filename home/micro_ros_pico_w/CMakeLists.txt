cmake_minimum_required(VERSION 3.12)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(micro_ros_pico_w C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_BOARD pico_w CACHE STRING "Board type")

pico_sdk_init()

link_directories(libmicroros)
add_executable(micro_ros_pico_w
    main.c
    pico_wifi_transport.c
)
target_link_libraries(micro_ros_pico_w
    pico_stdlib
    microros
    hardware_adc
)

target_include_directories(micro_ros_pico_w PUBLIC
    libmicroros/include
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
)

target_link_libraries(micro_ros_pico_w
    pico_cyw43_arch_lwip_threadsafe_background
)

SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")

# Configure Pico	#check next few lines function
pico_enable_stdio_usb(micro_ros_pico_w 1)
pico_enable_stdio_uart(micro_ros_pico_w 0)
add_compile_definitions(PICO_UART_ENABLE_CRLF_SUPPORT=0)
add_compile_definitions(PICO_STDIO_ENABLE_CRLF_SUPPORT=0)
add_compile_definitions(PICO_STDIO_DEFAULT_CRLF=0)

# Generate UF2
pico_add_extra_outputs(micro_ros_pico_w)

