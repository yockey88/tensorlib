
add_executable(neural_sandbox)

target_include_directories(neural_sandbox PRIVATE ${TENSORLIB_INCLUDES})
target_sources(neural_sandbox
  PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/neural_sandbox.cpp  
)