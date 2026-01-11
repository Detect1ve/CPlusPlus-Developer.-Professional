find_package(Eigen3 5.0.1 QUIET)
if (NOT Eigen_FOUND)
  include(FetchContent)
  set(BUILD_TESTING OFF CACHE BOOL "Disable Eigen tests" FORCE)
  FetchContent_Declare(
    Eigen3
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 5.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
  FetchContent_MakeAvailable(Eigen3)
endif()
