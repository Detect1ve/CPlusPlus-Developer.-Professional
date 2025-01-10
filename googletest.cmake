find_package(GTest 1.17.0)
if(NOT GTest_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    GTest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL)
  set(INSTALL_GTEST OFF CACHE BOOL "Disable installation of GTest" FORCE)
  FetchContent_MakeAvailable(GTest)
endif()
