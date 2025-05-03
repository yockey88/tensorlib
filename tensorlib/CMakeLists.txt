add_library(tensorlib)
set_target_properties(tensorlib PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(tensorlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(tensorlib
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tensorlib.cpp
)