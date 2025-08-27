include(${CMAKE_CURRENT_LIST_DIR}/../cmake/fetch_boost.cmake)

add_library(wrapper_boost_filesystem INTERFACE)
target_include_directories(wrapper_boost_filesystem
  INTERFACE ${CMAKE_CURRENT_LIST_DIR}/boost/filesystem/include)
target_link_libraries(wrapper_boost_filesystem INTERFACE Boost::filesystem)
