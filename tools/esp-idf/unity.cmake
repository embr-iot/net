# Specifically for unity tests, extra settings

target_compile_definitions(${COMPONENT_LIB}
    PRIVATE
        UNIT_TESTING=1)
