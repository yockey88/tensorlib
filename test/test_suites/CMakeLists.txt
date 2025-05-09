add_executable(test_suites)

target_include_directories(test_suites 
  PRIVATE 
      ${TENSORLIB_INCLUDES} 
      ${CMAKE_CURRENT_SOURCE_DIR}
)
target_sources(test_suites
  PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/tensorlib_testing.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/memory_tests/buffer_tests.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/memory_tests/arena_tests.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/vector_tests/vector_tests1.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/matrix_tests/matrix_tests1.cpp
)
target_link_libraries(test_suites PRIVATE tensorlib googlemock)