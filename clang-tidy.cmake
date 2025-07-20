option(ENABLE_CLANG_TIDY "Enable clang-tidy analysis" ON)
set(CLANG_TIDY_BIN "" CACHE
  STRING "Path to clang-tidy executable (empty for auto-detect)")

if (ENABLE_CLANG_TIDY)
  if (NOT CLANG_TIDY_BIN)
    find_program(CLANG_TIDY_BIN_FOUND
      NAMES
        clang-tidy
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
    set(CLANG_TIDY_COMMON_OPTS "${CLANG_TIDY_BIN};--use-color;--warnings-as-errors=*;-checks=*")
  else()
    message(STATUS "clang-tidy not found, static analysis will be skipped.")
  endif()
else()
  message(STATUS "clang-tidy disabled by user.")
endif()
