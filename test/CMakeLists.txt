add_executable(tensorlib_tests)
set_target_properties(tensorlib_tests PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(tensorlib_tests PRIVATE ${TENSORLIB_INCLUDES})
target_sources(tensorlib_tests
    PRIVATE
        main.cpp
)
target_link_libraries(tensorlib_tests PRIVATE tensorlib)