find_package(Microsoft.GSL 4.2.0 CONFIG)
if (NOT Microsoft.GSL_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    GSL
    GIT_REPOSITORY https://github.com/microsoft/GSL
    GIT_TAG v4.2.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL)

  FetchContent_MakeAvailable(GSL)
endif()
