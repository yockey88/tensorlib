add_library(test_comm1 SHARED)

target_compile_definitions(test_comm1 PRIVATE ${TENSORLIB_DEFINES} -DTENSORLIB_MODULE)
target_include_directories(test_comm1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${TENSORLIB_INCLUDES} ${NETWORK_INCLUDES})
target_sources(test_comm1
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/comm_layers/test_comm1.cpp
)
target_link_libraries(test_comm1 PRIVATE tensorlib network)