include(${CMAKE_CURRENT_LIST_DIR}/../cmake/fetch_eigen3.cmake)

add_library(eigen3_eigen INTERFACE)
target_include_directories(eigen3_eigen INTERFACE ${CMAKE_CURRENT_LIST_DIR}/eigen/include)
target_link_libraries(eigen3_eigen INTERFACE Eigen3::Eigen)
