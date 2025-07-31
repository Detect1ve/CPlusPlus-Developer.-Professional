find_package(absl)
if (NOT absl_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    absl
    GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
    GIT_TAG 20250512.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL)

  FetchContent_MakeAvailable(absl)
endif()
