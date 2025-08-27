include(${CMAKE_CURRENT_LIST_DIR}/../cmake/fetch_boost.cmake)

add_library(wrapper_boost_program_options INTERFACE)
target_include_directories(wrapper_boost_program_options
  INTERFACE ${CMAKE_CURRENT_LIST_DIR}/boost/program_options/include)
target_link_libraries(wrapper_boost_program_options INTERFACE Boost::program_options)
