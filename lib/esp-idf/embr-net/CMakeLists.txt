set(SOURCES
    flash.cpp
    net.cpp
    wifi.cpp)

# DEBT: PGESP-74 woes
list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/)

idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS
        "include"
        ${CMAKE_CURRENT_LIST_DIR}/../../lwip/include
    REQUIRES estd esp_wifi nvs_flash)
