include(${CMAKE_CURRENT_LIST_DIR}/../cmake/fetch_boost.cmake)

add_library(wrapper_boost_crc INTERFACE)
target_include_directories(wrapper_boost_crc
  INTERFACE ${CMAKE_CURRENT_LIST_DIR}/boost/crc/include)
target_link_libraries(wrapper_boost_crc INTERFACE Boost::crc)
