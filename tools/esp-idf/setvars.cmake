include(${CMAKE_CURRENT_LIST_DIR}/../cmake/setvars.cmake)

set(LIB_DIR ${ROOT_DIR}/lib/esp-idf)

if(NOT "$ENV{EMBR_NET_CONSOLE_INHIBIT}" STREQUAL "1")
    set(EMBR_NET_CONSOLE 1)
endif()