option(ENABLE_CPPCHECK "Enable cppcheck analysis" ON)
set(CPPCHECK_BIN "" CACHE STRING "Path to cppcheck executable (empty for auto-detect)")

if (ENABLE_CPPCHECK)
  if (NOT CPPCHECK_BIN)
    find_program(CPPCHECK_BIN_FOUND NAMES cppcheck NO_CACHE)
    if (CPPCHECK_BIN_FOUND)
      set(CPPCHECK_BIN "${CPPCHECK_BIN_FOUND}" CACHE
        STRING "Path to cppcheck executable (empty for auto-detect)" FORCE)
    endif()
  endif()

  if (CPPCHECK_BIN)
    message(STATUS "cppcheck enabled: ${CPPCHECK_BIN}")
    execute_process(COMMAND ${CPPCHECK_BIN} --version
                    OUTPUT_VARIABLE CPPCHECK_VERSION
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS "cppcheck version: ${CPPCHECK_VERSION}")
    set(CPPCHECK_BASE_ARGS
      # "--addon=cert"
      # "--addon=misra"
      "--check-level=exhaustive"
      "--enable=all"
      "--error-exitcode=1"
      "--inconclusive"
      "--suppress=missingIncludeSystem")
      # "-v")
      # "--inline-suppr"
  else()
    message(STATUS "cppcheck not found, static analysis will be skipped.")
  endif()
else()
  message(STATUS "cppcheck disabled by user.")
endif()

function(set_smart_cppcheck TARGET_NAME)
  if (NOT ENABLE_CPPCHECK OR NOT CPPCHECK_BIN)
    return()
  endif()

  cmake_parse_arguments(CPPCHECK
    ""
    "LIBRARIES"
    "EXCLUDE"
    ${ARGN})

  set(CPPCHECK_COMMAND "${CPPCHECK_BIN}")
  list(APPEND CPPCHECK_COMMAND ${CPPCHECK_BASE_ARGS})

  if (CPPCHECK_EXCLUDE)
    foreach(STR IN LISTS CPPCHECK_EXCLUDE)
      list(APPEND CPPCHECK_COMMAND "--suppress=${STR}")
    endforeach()
  endif()

  if (CPPCHECK_LIBRARIES)
    list(APPEND CPPCHECK_COMMAND "--library=${CPPCHECK_LIBRARIES}")
  endif()

  set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_CPPCHECK "${CPPCHECK_COMMAND}")
endfunction()
