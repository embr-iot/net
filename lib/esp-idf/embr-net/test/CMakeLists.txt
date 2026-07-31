idf_component_register(SRC_DIRS "."
    INCLUDE_DIRS "."
    REQUIRES
        embr-net
        cmock unity)

include(../../../../tools/esp-idf/unity.cmake)
