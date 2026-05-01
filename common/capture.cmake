include(${CMAKE_CURRENT_LIST_DIR}/../cmake/fetch_gsl.cmake)

add_library(capture INTERFACE)
target_include_directories(capture INTERFACE ${CMAKE_CURRENT_LIST_DIR}/capture/include)
target_link_libraries(capture INTERFACE Microsoft.GSL::GSL)
if (NOT MSVC)
  target_compile_options(capture INTERFACE -Wno-effc++)
endif()
