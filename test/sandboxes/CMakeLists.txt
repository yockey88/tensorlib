macro(add_sandbox name)
  add_executable(${name})
  target_include_directories(${name} PRIVATE ${TENSORLIB_INCLUDES})
  target_sources(${name}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/${name}.cpp  
  )
  target_link_libraries(${name} PRIVATE tensorlib)
endmacro()

add_sandbox(neural_sandbox)
add_sandbox(monte_carlo_sandbox)
add_sandbox(graph_sandbox)

# add_executable(neural_sandbox)
# target_include_directories(neural_sandbox PRIVATE ${TENSORLIB_INCLUDES})
# target_sources(neural_sandbox
#   PRIVATE
#       ${CMAKE_CURRENT_SOURCE_DIR}/neural_sandbox.cpp  
# )

# add_executable(monte_carlo_sandbox)
# target_include_directories(monte_carlo_sandbox PRIVATE ${TENSORLIB_INCLUDES})
# target_sources(monte_carlo_sandbox
#   PRIVATE
#       ${CMAKE_CURRENT_SOURCE_DIR}/monte_carlo_sandbox.cpp  
# )