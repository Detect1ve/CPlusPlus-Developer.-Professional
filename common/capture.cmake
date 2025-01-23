include(${CMAKE_CURRENT_LIST_DIR}/../cmake/fetch_gsl.cmake)

add_library(capture INTERFACE)
set(CAPTURE_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../common/capture/include)
target_include_directories(capture INTERFACE ${CAPTURE_INCLUDE_DIR})
target_link_libraries(capture INTERFACE Microsoft.GSL::GSL)
if (NOT MSVC)
  target_compile_options(capture INTERFACE -Wno-effc++)
endif()
