option(ENABLE_CLANG_TIDY "Enable clang-tidy analysis" ON)
set(CLANG_TIDY_BIN "" CACHE
  STRING "Path to clang-tidy executable (empty for auto-detect)")

if (ENABLE_CLANG_TIDY)
  if (NOT CLANG_TIDY_BIN)
    find_program(CLANG_TIDY_BIN_FOUND
      NAMES
        clang-tidy
        clang-tidy-22
        clang-tidy-21
        clang-tidy-20
        clang-tidy-19
        clang-tidy-18
        clang-tidy-17
        clang-tidy-16
        clang-tidy-15
        clang-tidy-14
        clang-tidy-13
        clang-tidy-12
        clang-tidy-11
        clang-tidy-10
        clang-tidy-9
        clang-tidy-8
        clang-tidy-7
        clang-tidy-6
        clang-tidy-5
        clang-tidy-4
        clang-tidy-3.9
        clang-tidy-3.8
      NO_CACHE)
    if (CLANG_TIDY_BIN_FOUND)
      set(CLANG_TIDY_BIN "${CLANG_TIDY_BIN_FOUND}" CACHE
        STRING "Path to clang-tidy executable (empty for auto-detect)" FORCE)
    endif()
  endif()

  if (CLANG_TIDY_BIN)
    message(STATUS "clang-tidy enabled: ${CLANG_TIDY_BIN}")
    execute_process(COMMAND ${CLANG_TIDY_BIN} --version
                    OUTPUT_VARIABLE CLANG_TIDY_VERSION
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS "clang-tidy version: ${CLANG_TIDY_VERSION}")
    string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" CLANG_TIDY_VERSION_NUMBER
      "${CLANG_TIDY_VERSION}")
    set(CLANG_TIDY_BASE_ARGS "")
    if (CLANG_TIDY_VERSION_NUMBER VERSION_GREATER_EQUAL 14)
      list(APPEND CLANG_TIDY_BASE_ARGS "--use-color")
    endif()

    list(APPEND CLANG_TIDY_BASE_ARGS "--warnings-as-errors=*")

    set(CLANG_TIDY_CHECKS_LIST
      "*"
      "-altera-id-dependent-backward-branch"
      "-altera-unroll-loops"
      "-clang-diagnostic-c++98-compat*"
      "-llvmlibc-*"
      "-modernize-use-trailing-return-type")
  else()
    message(STATUS "clang-tidy not found, static analysis will be skipped.")
  endif()
else()
  message(STATUS "clang-tidy disabled by user.")
endif()

function(set_smart_tidy TARGET_NAME)
  if (NOT ENABLE_CLANG_TIDY OR NOT CLANG_TIDY_BIN)
    return()
  endif()

  cmake_parse_arguments(TIDY
    "COGNITIVE_IGNORE_MACROS;HAS_EXCEPTIONS"
    "BUILD_PATH;MAIN_INCLUDE_DIR"
    "EXCLUDE;EXTRA_TARGETS"
    ${ARGN})

  set(TIDY_COMMAND "${CLANG_TIDY_BIN}")
  list(APPEND TIDY_COMMAND ${CLANG_TIDY_BASE_ARGS})

  set(CURRENT_CHECKS "${CLANG_TIDY_CHECKS_LIST}")
  if (TIDY_EXCLUDE)
    list(APPEND CURRENT_CHECKS ${TIDY_EXCLUDE})
  endif()

  list(JOIN CURRENT_CHECKS "," CHECKS_STR)
  list(APPEND TIDY_COMMAND "-checks=${CHECKS_STR}")

  if (TIDY_HAS_EXCEPTIONS AND MSVC)
    list(APPEND TIDY_COMMAND "--extra-arg=/EHsc")
  endif()

  if (MSVC)
    list(APPEND TIDY_COMMAND "--extra-arg=-Qunused-arguments")
  endif()

  list(APPEND TIDY_COMMAND "--extra-arg=-Wno-unknown-warning-option")

  if (NOT TIDY_MAIN_INCLUDE_DIR)
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include")
      set(BASE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
    endif()
  else()
    set(BASE_INCLUDE_DIR "${TIDY_MAIN_INCLUDE_DIR}")
  endif()

  set(FILTER_REGEX "")
  if (BASE_INCLUDE_DIR OR TIDY_EXTRA_TARGETS)
    set(FILTER_REGEX "(")

    if (BASE_INCLUDE_DIR)
      string(APPEND FILTER_REGEX "${BASE_INCLUDE_DIR}/.*")
    endif()

    set(ALL_EXTRA_INCLUDES "")

    foreach(TG IN LISTS TIDY_EXTRA_TARGETS)
      if (TARGET ${TG})
        get_target_property(TG_INCLUDES ${TG} INTERFACE_INCLUDE_DIRECTORIES)
        if (TG_INCLUDES AND NOT TG_INCLUDES MATCHES "-NOTFOUND$")
          list(APPEND ALL_EXTRA_INCLUDES ${TG_INCLUDES})
        endif()
      endif()
    endforeach()

    foreach(DIR IN LISTS ALL_EXTRA_INCLUDES)
      if (NOT FILTER_REGEX STREQUAL "(" AND NOT FILTER_REGEX MATCHES "\\|$")
        string(APPEND FILTER_REGEX "|")
      endif()
      string(APPEND FILTER_REGEX "${DIR}/.*")
    endforeach()

    string(APPEND FILTER_REGEX ")")
  endif()

  if (NOT FILTER_REGEX STREQUAL "()")
    list(APPEND TIDY_COMMAND "--header-filter=${FILTER_REGEX}")
  endif()

  if (TIDY_COGNITIVE_IGNORE_MACROS)
    list(APPEND TIDY_COMMAND "--config={CheckOptions: [{key: readability-function-cognitive-complexity.IgnoreMacros, value: 'true'}]}")
  endif()

  if (TIDY_BUILD_PATH)
    list(APPEND TIDY_COMMAND "-p" "${TIDY_BUILD_PATH}")
  endif()

  set_target_properties(${TARGET_NAME} PROPERTIES CXX_CLANG_TIDY "${TIDY_COMMAND}")
endfunction()
