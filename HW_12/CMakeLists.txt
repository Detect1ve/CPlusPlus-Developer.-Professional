cmake_minimum_required(VERSION 4.0.3)
project(HW_12 LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL)
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

add_custom_target(mapreduce_all ALL
  COMMAND make -f ${CMAKE_CURRENT_SOURCE_DIR}/Makefile all
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Building MapReduce using existing Makefile"
  VERBATIM
)

enable_testing()
add_test(
  NAME HW_12.test_mean_price
  COMMAND bash -c
  "cat ${CMAKE_CURRENT_SOURCE_DIR}/input/AB_NYC_2019.csv | \
  ${CMAKE_CURRENT_SOURCE_DIR}/bin/mapper | \
  sort -k1 | \
  ${CMAKE_CURRENT_SOURCE_DIR}/bin/reducer > ${CMAKE_CURRENT_BINARY_DIR}/output_mean && \
  grep -q 'Mean price: 152.696' ${CMAKE_CURRENT_BINARY_DIR}/output_mean"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
  NAME HW_12.test_variance
  COMMAND bash -c
  "cat ${CMAKE_CURRENT_SOURCE_DIR}/input/AB_NYC_2019.csv | \
  ${CMAKE_CURRENT_SOURCE_DIR}/bin/mapper_variance | \
  sort -k1 | \
  ${CMAKE_CURRENT_SOURCE_DIR}/bin/reducer_variance > \
    ${CMAKE_CURRENT_BINARY_DIR}/output_variance && \
  grep -q 'Variance: 57673.5' ${CMAKE_CURRENT_BINARY_DIR}/output_variance"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
