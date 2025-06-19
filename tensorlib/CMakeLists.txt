add_library(tensorlib)
set_target_properties(tensorlib PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(tensorlib PUBLIC ${TENSORLIB_INCLUDES})
target_sources(tensorlib
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/detail/util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/detail/tensorlib_state.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/core/ref.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/logger.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/random/rand.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/random/mcmc.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/core/arena.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/math/n_choose_k.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/linalgebra/dyn_vector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/linalgebra/dyn_matrix.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/graph.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/tensorlib.cpp
)
if (CMAKE_BUILD_TYPE STREQUAL Profile)
	target_sources(tensorlib PRIVATE ${TENSORLIB_EXTERN_DIR}/tracy/tracy/TracyClient.cpp)
	target_include_directories(tensorlib PUBLIC ${TENSORLIB_EXTERN_DIR}/tracy)
endif()
target_compile_definitions(tensorlib PRIVATE "${TENSORLIB_DEFINES}")