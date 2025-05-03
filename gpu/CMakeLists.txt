add_executable(tensorlib_gpu)
set_taregt_properties(tensorlib_gpu PROPERTIES LINKER_LANGUAGE CUDA)

target_sources(tensorlib_gpu
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/cuda_main.cu
)