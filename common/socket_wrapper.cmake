include(${CMAKE_CURRENT_LIST_DIR}/../cmake/fetch_absl.cmake)

add_library(socket_wrapper INTERFACE)
set(SOCKET_WRAPPER_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/socket_wrapper/include)
target_include_directories(socket_wrapper INTERFACE ${SOCKET_WRAPPER_INCLUDE_DIR})
target_link_libraries(socket_wrapper INTERFACE absl::strings)
