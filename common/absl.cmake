include(${CMAKE_CURRENT_LIST_DIR}/../cmake/fetch_absl.cmake)

add_library(wrapper_absl_strings INTERFACE)
target_include_directories(wrapper_absl_strings
  INTERFACE ${CMAKE_CURRENT_LIST_DIR}/absl/include)
target_link_libraries(wrapper_absl_strings INTERFACE absl::strings)
