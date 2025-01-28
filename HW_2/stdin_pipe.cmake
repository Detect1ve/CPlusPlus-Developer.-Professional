set(test_data_file INPUT_FILE test/ip_filter.tsv)

execute_process(COMMAND ${exe}
    ${test_data_file}
    OUTPUT_VARIABLE out
    ERROR_VARIABLE err
)

if(out MATCHES "TRACE")
    message(FATAL_ERROR "TRACE found in stdout")
endif()

if(err MATCHES "TRACE")
    message(FATAL_ERROR "TRACE found in stderr")
endif()

if(NOT err STREQUAL "")
    message(FATAL_ERROR "stderr output is not empty:
        ${err}")
endif()
