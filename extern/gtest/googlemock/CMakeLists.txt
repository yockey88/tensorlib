add_library(googlemock)

target_compile_options(googlemock PRIVATE -w)

target_include_directories(googlemock 
  PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(googlemock
  PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src/gmock-all.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/gmock-cardinalities.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/gmock-internal-utils.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/gmock-matchers.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/gmock-spec-builders.cc
      ${CMAKE_CURRENT_SOURCE_DIR}/src/gmock.cc
)

target_link_libraries(googlemock
  PUBLIC
      googletest
)