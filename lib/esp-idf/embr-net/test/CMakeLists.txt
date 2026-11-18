idf_component_register(SRC_DIRS "."
    INCLUDE_DIRS "."
    REQUIRES
        embr-net
        cmock unity)

include(../../../../tools/esp-idf/unity.cmake)


# For diagnostics only.  Generate .su stack usage reports
target_compile_options(${COMPONENT_LIB} PRIVATE -fstack-usage)
# "tell us whether GCC considered these objects and why it retained them."
target_compile_options(${COMPONENT_LIB} PRIVATE -fdump-tree-all-details)
