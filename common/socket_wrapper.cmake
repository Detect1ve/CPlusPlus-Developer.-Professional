add_library(socket_wrapper INTERFACE)
target_include_directories(socket_wrapper INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/socket_wrapper/include)
