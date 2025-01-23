include(${CMAKE_CURRENT_LIST_DIR}/../cmake/fetch_boost.cmake)

add_library(wrapper_boost_uuid INTERFACE)
target_include_directories(wrapper_boost_uuid
  INTERFACE ${CMAKE_CURRENT_LIST_DIR}/boost/uuid/include)
target_link_libraries(wrapper_boost_uuid INTERFACE Boost::uuid)
