cmake_minimum_required(VERSION 3.10.0..4.0.1 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(tensorlib LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(TENSORLIB_WINDOWS_BUILD false)
set(TENSORLIB_LINUX_BUILD false)
set(TENSORLIB_OSX_BUILD false)

set(TENSORLIB_DEFINES "")

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(TENSORLIB_WINDOWS_BUILD true)
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(TENSORLIB_LINUX_BUILD true)
elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(TENSORLIB_OSX_BUILD true)  
endif() 
  
if (TENSORLIB_WINDOWS_BUILD)
  message(STATUS "Building on Windows")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0A00") # Windows 10+
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWIN32_LEAN_AND_MEAN")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS")


  set(TENSORLIB_DEFINES ${TENSORLIB_DEFINES} TENSORLIB_WINDOWS)
elseif (TENSORLIB_LINUX_BUILD)
  message(STATUS "Building on Linux")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
  
  set(TENSORLIB_DEFINES ${TENSORLIB_DEFINES} TENSORLIB_LINUX)
elseif (TENSORLIB_OSX_BUILD)
  message(STATUS "Building on OSX")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

  set(TENSORLIB_DEFINES ${TENSORLIB_DEFINES} TENSORLIB_OSX)
else()
  message(FATAL_ERROR "Unknown OS")
endif()

set(TENSORLIB_EXTERN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern)
set(TENSORLIB_EXTERN_INCLUDES 
    ${TENSORLIB_EXTERN_DIR}
    ${TENSORLIB_EXTERN_DIR}/flatbuffers
    ${TENSORLIB_EXTERN_DIR}/magic_enum
    ${TENSORLIB_EXTERN_DIR}/asio
)

set(TENSORLIB_EXTERN_LIBRARY_DIRS ${TENSORLIB_EXTERN_DIR}/flatbuffers/lib)

set(TENSORLIB_DEFINES ${TENSORLIB_DEFINES} ASIO_STANDALONE)
set(TENSORLIB_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/tensorlib ${TENSORLIB_EXTERN_INCLUDES})
set(NEURAL_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/neural)
set(NETWORK_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/network)

option(TENSORLIB_REAL_TYPE_USE_DOUBLE "Use doubles for tensor values" OFF)

macro (add_library_and_driver ${name} ${ARGN})
  set(LIB_SOURCE_FILES_LIST "${ARGN}")
  add_library(${name})
  # set(SOURCE_FILES "")

  set(SRC_FILE_STR "")
  foreach (SRC_FILE IN LISTS LIB_SOURCE_FILES_LIST)
    list(APPEND SRC_FILE_STR "${SRC_FILE}")
  endforeach()

  target_sources(${name}
    PRIVATE
      ${SRC_FILE_STR}
  )
  target_compile_definitions(${name} PRIVATE ${TENSORLIB_DEFINES})
  target_include_directories(${name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(${name} PUBLIC tensorlib)

  add_executable(${name}_driver)

  target_include_directories(${name}_driver PRIVATE ${TENSORLIB_INCLUDES} ${DRIVERLIB_INCLUDES})
  target_sources(${name}_driver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${name}.cpp)
  target_link_libraries(${name}_driver PRIVATE tensorlib network)

  # # target_compile_definitions(network
  # #   PRIVATE
  # #     TENSORLIB_EXTERN_DIR="${TENSORLIB_EXTERN_DIR}"
  # #     TENSORLIB_WINDOWS_BUILD=${TENSORLIB_WINDOWS_BUILD}
  # #     TENSORLIB_LINUX_BUILD=${TENSORLIB_LINUX_BUILD}
  # #     TENSORLIB_OSX_BUILD=${TENSORLIB_OSX_BUILD}
  # # )
endmacro()

### add dependencies
find_package(Threads REQUIRED)

add_subdirectory(extern)

add_subdirectory(tensorlib)
# add_subdirectory(gpu)
add_subdirectory(neural)
add_subdirectory(network)

add_subdirectory(test)
