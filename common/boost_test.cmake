include(${CMAKE_CURRENT_LIST_DIR}/../cmake/fetch_boost.cmake)

add_library(wrapper_boost_unit_test_framework INTERFACE)
target_include_directories(wrapper_boost_unit_test_framework
  INTERFACE ${CMAKE_CURRENT_LIST_DIR}/boost/test/include)
target_link_libraries(wrapper_boost_unit_test_framework
  INTERFACE Boost::unit_test_framework)
