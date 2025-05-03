add_library(googletest)

target_compile_options(googletest PRIVATE -w)

target_include_directories(googletest 
  PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(googletest
  PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src/gtest-all.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/gtest-assertion-result.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/gtest-death-test.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/gtest-filepath.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/gtest-matchers.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/gtest-port.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/gtest-printers.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/gtest-test-part.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/gtest-typed-test.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/gtest.cc
)