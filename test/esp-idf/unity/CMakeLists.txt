cmake_minimum_required(VERSION 3.22)

set(TEST_COMPONENTS "embr-net" CACHE STRING "List of components for Unity to pick up")

set(COMPONENTS main ${TEST_COMPONENTS})

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(embr-net-unity)
