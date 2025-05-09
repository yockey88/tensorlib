add_executable(neural)

target_sources(neural
  PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/driver.cpp
)
target_link_libraries(neural PRIVATE tensorlib)