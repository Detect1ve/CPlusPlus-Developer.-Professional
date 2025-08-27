add_library(capture INTERFACE)
set(CAPTURE_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../common/capture/include)
target_include_directories(capture INTERFACE ${CAPTURE_INCLUDE_DIR})
target_compile_options(capture INTERFACE -Wno-effc++)
