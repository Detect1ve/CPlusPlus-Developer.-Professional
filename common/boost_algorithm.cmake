include(${CMAKE_CURRENT_LIST_DIR}/../cmake/fetch_boost.cmake)

add_library(wrapper_boost_algorithm INTERFACE)
target_include_directories(wrapper_boost_algorithm
  INTERFACE ${CMAKE_CURRENT_LIST_DIR}/boost/algorithm/include)
target_link_libraries(wrapper_boost_algorithm INTERFACE Boost::algorithm)
