# CMakeLists dormant and untested

include({$CMAKE_CURRENT_LIST_DIR}/sources.cmake)

# DEBT: PGESP-74 woes
list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/)

idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS
        "include"
    REQUIRES embr-net console)
