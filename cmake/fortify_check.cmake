include_guard(GLOBAL)

function(check_fortify_source_support)
  if (DEFINED CACHE{FORTIFY_CHECK_DONE})
    return()
  endif()

  if (MSVC)
    message(STATUS "[Fortify] Skipping _FORTIFY_SOURCE check for MSVC (not supported)")
    set(FORTIFY_SUPPORTED FALSE CACHE INTERNAL "Fortify supported")
    set(FORTIFY_CHECK_DONE TRUE CACHE INTERNAL "Fortify check done")
    return()
  endif()

  string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_UPPER)
  set(FLAGS_TO_TEST "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${BUILD_TYPE_UPPER}}")

  get_property(DIR_OPTIONS DIRECTORY PROPERTY COMPILE_OPTIONS)
  get_property(DIR_DEFS DIRECTORY PROPERTY COMPILE_DEFINITIONS)

  foreach(opt ${DIR_OPTIONS})
    string(APPEND FLAGS_TO_TEST " ${opt}")
  endforeach()

  foreach(def ${DIR_DEFS})
    string(APPEND FLAGS_TO_TEST " -D${def}")
  endforeach()

  set(FORTIFY_CHECK_CODE
    "#include <stdio.h>
    int main() {
      int level = 0;
      int optimized = 0;
    #ifdef _FORTIFY_SOURCE
      level = _FORTIFY_SOURCE;
    #endif
    #ifdef __OPTIMIZE__
      optimized = 1;
    #endif
      printf(\"%d|%d\", level, optimized);

      return 0;
    }")

  try_run(
    RUN_RESULT
    COMPILE_RESULT
    SOURCE_FROM_CONTENT "fortify_test.cpp" "${FORTIFY_CHECK_CODE}"
    COMPILE_DEFINITIONS ${FLAGS_TO_TEST}
    RUN_OUTPUT_VARIABLE TEST_RAW_OUTPUT
    COMPILE_OUTPUT_VARIABLE COMPILE_LOG)

  if (NOT COMPILE_RESULT OR NOT RUN_RESULT EQUAL 0)
    message(STATUS "[Fortify] Could not run check (Compile error or cross-compilation).")
    set(FORTIFY_SUPPORTED FALSE CACHE INTERNAL "Fortify supported")
  else()
    string(REPLACE "|" ";" TEST_RES_LIST "${TEST_RAW_OUTPUT}")
    list(GET TEST_RES_LIST 0 DETECTED_LEVEL)
    list(GET TEST_RES_LIST 1 IS_OPTIMIZED)

    set(FORTIFY_DETECTED_LEVEL ${DETECTED_LEVEL} CACHE INTERNAL "Detected _FORTIFY_SOURCE level")
    set(FORTIFY_IS_OPTIMIZED ${IS_OPTIMIZED} CACHE INTERNAL "Is optimization enabled")
    set(FORTIFY_SUPPORTED TRUE CACHE INTERNAL "Fortify supported")
  endif()

  set(FORTIFY_CHECK_DONE TRUE CACHE INTERNAL "Fortify check done")
endfunction()

function(apply_fortify_source TARGET_NAME)
  check_fortify_source_support()

  if (NOT FORTIFY_SUPPORTED)
    return()
  endif()

  if (NOT FORTIFY_IS_OPTIMIZED)
    message(STATUS "[Fortify] Optimization is disabled for ${TARGET_NAME}. Fortify skipped as it requires -O1 or higher.")
    return()
  endif()

  if (FORTIFY_DETECTED_LEVEL EQUAL 3)
    message(WARNING "[Fortify] Compiler implicitly set _FORTIFY_SOURCE to 3 for ${TARGET_NAME}! This is unexpected.")
  else()
    if (FORTIFY_DETECTED_LEVEL GREATER 0)
      message(WARNING "[Fortify] The current _FORTIFY_SOURCE is implicitly ${DETECTED_LEVEL}. Upgrading to 3 for ${TARGET_NAME}")
    else()
      message(STATUS "[Fortify] Enabling _FORTIFY_SOURCE=3 for ${TARGET_NAME}")
    endif()

    target_compile_options(${TARGET_NAME} PRIVATE
      $<$<AND:$<NOT:$<CONFIG:Debug>>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-U_FORTIFY_SOURCE;-D_FORTIFY_SOURCE=3>)
  endif()
endfunction()
