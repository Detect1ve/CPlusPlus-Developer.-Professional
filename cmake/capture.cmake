add_library(capture INTERFACE)
get_filename_component(CAPTURE_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../common/capture
  REALPATH)
target_include_directories(capture INTERFACE ${CAPTURE_INCLUDE_DIR})
