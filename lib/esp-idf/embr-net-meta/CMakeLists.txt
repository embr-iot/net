# I don't think we can do this in a component, but let's give it a shot
LIST(APPEND EXTRA_COMPONENT_DIRS ${CMAKE_CURRENT_LIST_DIR}/..)

idf_component_register(SRCS "dummy.cpp"
    REQUIRES embr-net)
