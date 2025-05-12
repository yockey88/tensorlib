add_library(neural)

target_include_directories(neural PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(neural
  PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/neural/activation.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/neural/ann.cpp
)
target_link_libraries(neural PUBLIC tensorlib)

add_executable(neural_driver)

target_include_directories(neural_driver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(neural_driver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/driver.cpp)
target_link_libraries(neural_driver PRIVATE tensorlib neural)