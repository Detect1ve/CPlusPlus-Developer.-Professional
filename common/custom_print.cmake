add_library(custom_print INTERFACE)
set(CUSTOM_PRINT_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/custom_print/include)
target_include_directories(custom_print INTERFACE ${CUSTOM_PRINT_INCLUDE_DIR})
