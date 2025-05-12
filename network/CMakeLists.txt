add_library(network)

target_include_directories(network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(network
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/detail/netw_util.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/message/message.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/network/link_based_network.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/simulation/timer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/simulation/library_loader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/simulation/dyn_library.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/simulation/simulation_config.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/simulation/simulation_event.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/simulation/simulation_event_handler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/simulation/simulation_node.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/simulation/simulation_control.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/simulation/simulation_state.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/simulation/simulation.cpp
)
target_link_libraries(network PUBLIC tensorlib flatbuffers)

if (TENSORLIB_WINDOWS_BUILD)
    target_sources(network
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/simulation/platform/windows_dyn_library.cpp    
            ${CMAKE_CURRENT_SOURCE_DIR}/simulation/platform/windows_library_loader.cpp    
    )
endif()

# target_compile_definitions(network
#   PRIVATE
#     TENSORLIB_EXTERN_DIR="${TENSORLIB_EXTERN_DIR}"
#     TENSORLIB_WINDOWS_BUILD=${TENSORLIB_WINDOWS_BUILD}
#     TENSORLIB_LINUX_BUILD=${TENSORLIB_LINUX_BUILD}
#     TENSORLIB_OSX_BUILD=${TENSORLIB_OSX_BUILD}
# )

add_executable(network_driver)

target_include_directories(network_driver PRIVATE ${TENSORLIB_INCLUDES} ${NETWORK_INCLUDES})
target_sources(network_driver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/network.cpp)
target_link_directories(tensorlib PUBLIC ${TENSORLIB_EXTERN_LIBRARY_DIRS})
target_link_libraries(network_driver PRIVATE tensorlib network)


# add layer dlls
add_subdirectory(simulations)