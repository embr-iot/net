cmake_minimum_required(VERSION 3.22)

include(../../../tools/esp-idf/setenv.cmake)

# 31JUL26 - I don't recall EXTRA_COMPONENT_DIRS being necessary for
# unity before (thought idf_component.yml could stand in) but it seems to be now 
set(EXTRA_COMPONENT_DIRS ../../../lib/esp-idf)
set(TEST_COMPONENTS "embr-net" CACHE STRING "List of components for Unity to pick up")

set(COMPONENTS main ${TEST_COMPONENTS})

project(embr-net-unity)
