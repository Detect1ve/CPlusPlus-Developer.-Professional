include(${CMAKE_CURRENT_LIST_DIR}/absl.cmake)

add_library(socket_wrapper INTERFACE)
get_filename_component(SOCKET_WRAPPER_INCLUDE_DIR
  ${CMAKE_CURRENT_LIST_DIR}/../common/socket_wrapper REALPATH)
target_include_directories(socket_wrapper INTERFACE ${SOCKET_WRAPPER_INCLUDE_DIR})

target_link_libraries(socket_wrapper INTERFACE absl::strings)
