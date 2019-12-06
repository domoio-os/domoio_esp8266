cmake_minimum_required (VERSION 2.8.8)
project (domoio)

set (PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

IF(NOT ESP_ROOT)
  set(ESP_ROOT ${PROJECT_ROOT}/../esp8266)
ENDIF(NOT ESP_ROOT)

IF(NOT AXTLS_ROOT)
  set(AXTLS_ROOT ${PROJECT_ROOT}/../axtls-8266)
ENDIF(NOT AXTLS_ROOT)

message("-- Settings")
message("     ESP_ROOT " ${ESP_ROOT})
message("     AXTLS_ROOT " ${AXTLS_ROOT})

set(XTENSA_BIN ${ESP_ROOT}/tools/xtensa-lx106-elf/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x -Wno-deprecated -Os -g -mlongcalls -mtext-section-literals -fno-exceptions -fno-rtti -falign-functions=4 -MMD -ffunction-sections -fdata-sections" )

add_definitions( -DARDUINO=10605 )
add_definitions( -DLWIP_OPEN_SRC )


SET(CMAKE_CXX_COMPILER ${XTENSA_BIN}/xtensa-lx106-elf-g++)
SET(CMAKE_LINKER ${XTENSA_BIN}/xtensa-lx106-elf-ld)
SET(CMAKE_RANLIB ${XTENSA_BIN}/xtensa-lx106-elf-ranlib)
SET(CMAKE_AR ${XTENSA_BIN}/xtensa-lx106-elf-ar)

include_directories(
  ${ESP_ROOT}/cores/esp8266
  ${ESP_ROOT}/libraries/EEPROM

  ${ESP_ROOT}/libraries/ESP8266httpUpdate/src
  ${ESP_ROOT}/libraries/ESP8266WiFi/src

  ${ESP_ROOT}/libraries/ESP8266HTTPClient/src
  ${ESP_ROOT}/libraries/ESP8266WebServer/src
  ${ESP_ROOT}/libraries/DNSServer/src
  ${ESP_ROOT}/libraries/Ticker/src
  ${ESP_ROOT}/libraries/ArduinoOTA

  ${ESP_ROOT}/tools/sdk/include
  ${ESP_ROOT}/tools/sdk/lwip/include
  ${ESP_ROOT}/variants/generic
  ${AXTLS_ROOT}
  ${AXTLS_ROOT}/crypto
)


set(SRC ${SRC}
  ./src/actions
  ./src/crypto
  ./src/base64
  ./src/flash
  ./src/led
  ./src/message
  ./src/reactduino
  ./src/tools
  ./src/wificonf
  ./src/cantcoap
  ./src/domoio
  ./src/main
  ./src/port
  ./src/storage
  ./src/watcher
)



# Create the shared library
#-----------------------------------------------------------------------

add_library (domoio ${SRC})
target_link_libraries (domoio)
