include(${CMAKE_CURRENT_LIST_DIR}/../cmake/fetch_gsl.cmake)

add_library(capture INTERFACE)
set(CAPTURE_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/capture/include)
target_include_directories(capture INTERFACE ${CAPTURE_INCLUDE_DIR})
target_link_libraries(capture INTERFACE Microsoft.GSL::GSL)
set_warning_flags(capture
  EXTRA_GCC -Wno-effc++
  NO_BASE
  SCOPE "INTERFACE")
