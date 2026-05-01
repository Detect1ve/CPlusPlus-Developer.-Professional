add_library(custom_print INTERFACE)
target_include_directories(custom_print INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/custom_print/include)
