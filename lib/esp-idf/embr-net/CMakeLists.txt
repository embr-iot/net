include(${CMAKE_CURRENT_LIST_DIR}/../../../tools/esp-idf/setvars.cmake)

include(${ROOT_DIR}/src/sources.cmake)

list(TRANSFORM SOURCES PREPEND ${ROOT_DIR}/src/ OUTPUT_VARIABLE core_sources)

if(EMBR_NET_CONSOLE)
    # DEBT: PGESP-74 means we have to bake embr-net-console into embr-net.
    # Usually nobody probably will care, but inc ase you do one can specify
    # EMBR_NET_CONSOLE_INHIBIT in environment
    include(${LIB_DIR}/embr-net-console/sources.cmake)

    list(TRANSFORM SOURCES PREPEND ${LIB_DIR}/embr-net-console/)

    set(EMBR_NET_CONSOLE_INCLUDE_DIRS ${LIB_DIR}/embr-net-console/include)
    set(EMBR_NET_CONSOLE_REQUIRES console)
endif()

set(MY_SOURCES
    flash.cpp
    net.cpp
    wifi.cpp)

# DEBT: PGESP-74 woes, so that top-level idf_component.yml doesn't get confused
list(TRANSFORM MY_SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/)

idf_component_register(
    SRCS ${SOURCES} ${CORE_SOURCES} ${MY_SOURCES}
    INCLUDE_DIRS
        "include"
        ${EMBR_NET_CONSOLE_INCLUDE_DIRS}
        ${ROOT_DIR}/lib/lwip/include
        ${ROOT_DIR}/src/include
    REQUIRES
        ${EMBR_NET_CONSOLE_REQUIRES}
        estd esp_wifi nvs_flash
        )
