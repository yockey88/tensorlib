add_library(tensorlib)
set_target_properties(tensorlib PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(tensorlib PUBLIC ${TENSORLIB_INCLUDES})
target_sources(tensorlib
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/detail/tensorlib_state.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/core/ref.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/core/rand.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/core/arena.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/linalgebra/dyn_vector.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/linalgebra/dyn_matrix.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/neural/activation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/neural/ann.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tensorlib.cpp
)