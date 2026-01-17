cmake_minimum_required(VERSION 3.25..4.2.1)

if (NOT DEFINED PATCH_VERSION)
  set(PATCH_VERSION
      "1"
      CACHE INTERNAL "Patch version")
endif()

set(PROJECT_VERSION 0.0.${PATCH_VERSION})
project(mapreduce VERSION ${PROJECT_VERSION} LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
  include(../cmake/clang-tidy.cmake)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

if (MSVC OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if (PROJECT_IS_TOP_LEVEL)
    if (NOT MSVC)
      add_compile_options(-fcolor-diagnostics)
    endif()

    set(MINIMUM_CXX_STANDARD 17)
    include(../cmake/cxx_standard.cmake)
    include(../cmake/warning_flags.cmake)
  endif()

  if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(HW_12_COMPILE_WARNING_FLAGS -Wc++17-compat-pedantic)
  endif()

  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

  add_executable(mapper mapper.cpp)
  add_executable(reducer reducer.cpp)
  add_executable(mapper_variance mapper_variance.cpp)
  add_executable(reducer_variance reducer_variance.cpp)

  if (NOT MSVC)
    target_link_libraries(mapper PRIVATE -static)
    target_link_libraries(reducer PRIVATE -static)
    target_link_libraries(mapper_variance PRIVATE -static)
    target_link_libraries(reducer_variance PRIVATE -static)
  endif()

  target_compile_options(mapper PRIVATE
    ${COMPILE_WARNING_FLAGS} ${HW_12_COMPILE_WARNING_FLAGS})
  target_compile_options(reducer PRIVATE
    ${COMPILE_WARNING_FLAGS} ${HW_12_COMPILE_WARNING_FLAGS})
  target_compile_options(mapper_variance PRIVATE
    ${COMPILE_WARNING_FLAGS} ${HW_12_COMPILE_WARNING_FLAGS})
  target_compile_options(reducer_variance PRIVATE
    ${COMPILE_WARNING_FLAGS} ${HW_12_COMPILE_WARNING_FLAGS})
  if (NOT MSVC)
    target_compile_options(mapper PRIVATE -Wno-unused-parameter -Wno-unsafe-buffer-usage)
    target_compile_options(reducer PRIVATE -Wno-unused-parameter)
    target_compile_options(mapper_variance PRIVATE -Wno-unsafe-buffer-usage)
  endif()

  if (ENABLE_CLANG_TIDY AND CLANG_TIDY_BIN)
    set(CLANG_TIDY_HW_12_OPTS
      "-fuchsia-default-arguments-calls,\
       -llvmlibc-callee-namespace,\
       -llvmlibc-implementation-in-namespace,\
       -llvmlibc-restrict-system-libc-headers,\
       -misc-include-cleaner,\
       -modernize-use-trailing-return-type")

    set_target_properties(mapper PROPERTIES
      CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
        -clang-diagnostic-unused-parameter,\
        -llvm-prefer-static-over-anonymous-namespace,\
        -misc-unused-parameters,\
        -performance-avoid-endl,\
        ${CLANG_TIDY_HW_12_OPTS}")

    set_target_properties(reducer PROPERTIES
      CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
        -clang-diagnostic-unused-parameter,\
        -misc-unused-parameters,\
        -performance-avoid-endl,\
        ${CLANG_TIDY_HW_12_OPTS}")

    set_target_properties(mapper_variance PROPERTIES
      CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
        -llvm-prefer-static-over-anonymous-namespace,\
        ${CLANG_TIDY_HW_12_OPTS}")

    set_target_properties(reducer_variance PROPERTIES
      CXX_CLANG_TIDY "${CLANG_TIDY_OPTS},\
        ${CLANG_TIDY_HW_12_OPTS}")
  endif()
else()
  add_custom_target(mapreduce_all ALL
    COMMAND make -f ${CMAKE_CURRENT_SOURCE_DIR}/Makefile all
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building MapReduce using existing Makefile"
    VERBATIM)
endif()

enable_testing()

if (WIN32)
  add_test(
    NAME HW_12.test_mean_price
    COMMAND powershell -Command
    "cat ${CMAKE_CURRENT_SOURCE_DIR}/input/AB_NYC_2019.csv | \
    $<TARGET_FILE:mapper> | \
    sort | \
    $<TARGET_FILE:reducer> > ${CMAKE_CURRENT_BINARY_DIR}/output_mean ; \
    if (-not (Select-String -Pattern 'Mean price: 152.696' \
      -Path ${CMAKE_CURRENT_BINARY_DIR}/output_mean)) \
    { \
      Write-Host 'ERROR: Pattern not found: Mean price: 152.696' ; \
      Write-Host '--- Output file content:' ; \
      cat ${CMAKE_CURRENT_BINARY_DIR}/output_mean ; \
      exit 1 \
    }"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

  add_test(
    NAME HW_12.test_variance
    COMMAND powershell -Command
    "cat ${CMAKE_CURRENT_SOURCE_DIR}/input/AB_NYC_2019.csv | \
    $<TARGET_FILE:mapper_variance> | \
    sort | \
    $<TARGET_FILE:reducer_variance> > ${CMAKE_CURRENT_BINARY_DIR}/output_variance ; \
    if (-not (Select-String -Pattern 'Variance: 57673.5' \
      -Path ${CMAKE_CURRENT_BINARY_DIR}/output_variance)) \
    { \
      Write-Host 'ERROR: Pattern not found: Variance: 57673.5' ; \
      Write-Host '--- Output file content:' ; \
      cat ${CMAKE_CURRENT_BINARY_DIR}/output_variance ; \
      exit 1 \
    }"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
else()
  add_test(
    NAME HW_12.test_mean_price
    COMMAND bash -c
    "cat ${CMAKE_CURRENT_SOURCE_DIR}/input/AB_NYC_2019.csv | \
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/mapper | \
    sort -k1 | \
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/reducer > ${CMAKE_CURRENT_BINARY_DIR}/output_mean ; \
    if ! grep -q 'Mean price: 152.696' ${CMAKE_CURRENT_BINARY_DIR}/output_mean; then \
      echo 'ERROR: Pattern not found: Mean price: 152.696' ; \
      echo '--- Output file content:' ; \
      cat ${CMAKE_CURRENT_BINARY_DIR}/output_mean ; \
      exit 1 ; \
    fi"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

  add_test(
    NAME HW_12.test_variance
    COMMAND bash -c
    "cat ${CMAKE_CURRENT_SOURCE_DIR}/input/AB_NYC_2019.csv | \
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/mapper_variance | \
    sort -k1 | \
    ${CMAKE_CURRENT_SOURCE_DIR}/bin/reducer_variance > \
      ${CMAKE_CURRENT_BINARY_DIR}/output_variance ; \
    if ! grep -q 'Variance: 57673.5' ${CMAKE_CURRENT_BINARY_DIR}/output_variance; then \
      echo 'ERROR: Pattern not found: Variance: 57673.5' ; \
      echo '--- Output file content:' ; \
      cat ${CMAKE_CURRENT_BINARY_DIR}/output_variance ; \
      exit 1 ; \
    fi"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/
        DESTINATION bin
        USE_SOURCE_PERMISSIONS
        COMPONENT "${PROJECT_NAME}")
if (PROJECT_IS_TOP_LEVEL)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_CONTACT Nikolai_Grin@Mail.Ru)
  include(CPack)
else()
  string(TOUPPER "${PROJECT_NAME}" PROJECT_NAME_UPPER)

  set(CPACK_DEBIAN_${PROJECT_NAME_UPPER}_FILE_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)

  set(CPACK_DEBIAN_${PROJECT_NAME_UPPER}_PACKAGE_NAME
      "${PROJECT_NAME}-${PROJECT_VERSION}-Linux.deb"
      PARENT_SCOPE)
endif()
