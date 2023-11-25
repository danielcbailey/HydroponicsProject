cmake_minimum_required(VERSION 3.12)
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

project(HydroponicsBox C CXX ASM)
include(PicoSDKConfig.cmake)
pico_sdk_init()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
add_executable(HydroponicsBox
        main.cpp
        graphicLCD.cpp
        assets/font.cpp
        RotaryEncoder.cpp
        menus/Menu.cpp
        inputs.cpp
        DisplayManager.cpp
        menus/splashPage.cpp
        sensors.cpp
        ${PICO_SDK_PATH}/src/rp2_common/hardware_i2c/i2c.c
        communication.cpp
        menus/MainMenu.cpp
        menus/phCalibrationMenu.cpp
        serverComm.cpp
        cjson/cJSON.c
        schedule.cpp)

pico_generate_pio_header(HydroponicsBox ${CMAKE_CURRENT_LIST_DIR}/rotaryEncoder.pio)
target_include_directories(HydroponicsBox PRIVATE . ${PICO_SDK_PATH}/src/rp2_common/hardware_pio/include menu ${PICO_SDK_PATH}/src/rp2_common/hardware_i2c/include ../../../../../AppData/Local/VisualGDB/PicoSDK/1.5.1-Package/src/rp2_common/pico_multicore/include ../../../../../AppData/Local/VisualGDB/PicoSDK/1.5.1-Package/src/rp2_common/hardware_sync/include ../../../../../AppData/Local/VisualGDB/PicoSDK/1.5.1-Package/src/rp2_common/hardware_irq/include ../../../../../AppData/Local/VisualGDB/PicoSDK/1.5.1-Package/src/rp2_common/hardware_claim/include ${PICO_SDK_PATH}/src/rp2_common/hardware_rtc/include)
target_include_directories(HydroponicsBox PRIVATE .)
# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(HydroponicsBox pico_stdlib)
target_link_libraries(HydroponicsBox hardware_pio)
target_link_libraries(HydroponicsBox pico_multicore pico_stdlib)

# create map/bin/hex file etc.
pico_add_extra_outputs(HydroponicsBox)

# add url via pico_set_program_url
# example_auto_set_url(picoboard_button)

pico_enable_stdio_usb(HydroponicsBox 1)
pico_enable_stdio_uart(HydroponicsBox 0)
