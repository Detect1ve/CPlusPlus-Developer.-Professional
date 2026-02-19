get_property(DIR_OPTIONS DIRECTORY PROPERTY COMPILE_OPTIONS)
get_property(DIR_DEFS DIRECTORY PROPERTY COMPILE_DEFINITIONS)

string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_UPPER)
set(FLAGS_TO_TEST "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${BUILD_TYPE_UPPER}}")

foreach(opt ${DIR_OPTIONS})
  string(APPEND FLAGS_TO_TEST " ${opt}")
endforeach()

foreach(def ${DIR_DEFS})
  string(APPEND FLAGS_TO_TEST " -D${def}")
endforeach()

set(FORTIFY_TEST_DIR "${CMAKE_BINARY_DIR}/fortify_test_dir")
file(MAKE_DIRECTORY "${FORTIFY_TEST_DIR}")
set(FORTIFY_TEST_FILE "${FORTIFY_TEST_DIR}/test_fortify.cpp")

file(WRITE "${FORTIFY_TEST_FILE}"
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
    "${FORTIFY_TEST_DIR}"
    "${FORTIFY_TEST_FILE}"
    COMPILE_DEFINITIONS ${FLAGS_TO_TEST}
    RUN_OUTPUT_VARIABLE TEST_RAW_OUTPUT
    COMPILE_OUTPUT_VARIABLE COMPILE_LOG)

if (COMPILE_RESULT AND RUN_RESULT EQUAL 0)
  string(REPLACE "|" ";" TEST_RES_LIST "${TEST_RAW_OUTPUT}")
  list(GET TEST_RES_LIST 0 DETECTED_LEVEL)
  list(GET TEST_RES_LIST 1 IS_OPTIMIZED)

  if (DETECTED_LEVEL GREATER 0)
    if (IS_OPTIMIZED)
      message(STATUS ">>> _FORTIFY_SOURCE = ${DETECTED_LEVEL} (ACTIVE) <<<")
    else()
      message(STATUS ">>> _FORTIFY_SOURCE = ${DETECTED_LEVEL} (INACTIVE: No optimization/Debug) <<<")
    endif()
  else()
    message(STATUS ">>> _FORTIFY_SOURCE is NOT defined <<<")
  endif()
else()
  message(STATUS ">>> Could not run Fortify check (Compile error or cross-compilation) <<<")
endif()
