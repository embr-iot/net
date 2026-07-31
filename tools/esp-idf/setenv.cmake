if(EXISTS "${CMAKE_SOURCE_DIR}/sdkconfig.local")
    message(DEBUG "Picking up local config")
    # Use this to configure things like dev-specific sensitive WiFi SSID and password, etc.
    list(APPEND SDKCONFIG_DEFAULTS sdkconfig.local)
endif()

set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
