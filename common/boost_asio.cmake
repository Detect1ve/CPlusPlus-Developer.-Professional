include(${CMAKE_CURRENT_LIST_DIR}/../cmake/fetch_boost.cmake)

add_library(wrapper_boost_asio INTERFACE)
target_include_directories(wrapper_boost_asio
  INTERFACE ${CMAKE_CURRENT_LIST_DIR}/boost/asio/include)
target_link_libraries(wrapper_boost_asio INTERFACE Boost::asio)
