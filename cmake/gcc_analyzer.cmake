include(${CMAKE_CURRENT_LIST_DIR}/patch_compile_commands.cmake)

option(ENABLE_ANALYZER "Enable GCC static analyzer (-fanalyzer)" ON)

if (  ENABLE_ANALYZER
  AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
  AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 10.0)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

function(set_analyzer TARGET_NAME)
  if (NOT ENABLE_ANALYZER)
    return()
  endif()

  if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    return()
  endif()

  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0)
    message(WARNING "GCC version ${CMAKE_CXX_COMPILER_VERSION} is too old for -fanalyzer (need 10.0+)")
    return()
  endif()

  cmake_parse_arguments(ANALYZER
    ""
    ""
    "EXCLUDE;EXCLUDE_RELEASE;EXCLUDE_MINSIZEREL"
    ${ARGN})

  target_compile_options(${TARGET_NAME} PRIVATE -fanalyzer)

  set(LOCAL_ANALYZER_FLAGS "")

  foreach(EX IN LISTS ANALYZER_EXCLUDE)
    if (EX MATCHES "^-W")
      list(APPEND LOCAL_ANALYZER_FLAGS "${EX}")
    else()
      list(APPEND LOCAL_ANALYZER_FLAGS "-Wno-analyzer-${EX}")
    endif()
  endforeach()

  target_compile_options(${TARGET_NAME} PRIVATE
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<NOT:$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>>>:${LOCAL_ANALYZER_FLAGS}>)

  function(_apply_analyzer_config_exclusions CONFIG_VAL EX_LIST)
    foreach(EX IN LISTS ${EX_LIST})
      set(FLAG "${EX}")
      if (NOT FLAG MATCHES "^-W")
        set(FLAG "-Wno-analyzer-${EX}")
      endif()
      target_compile_options(${TARGET_NAME} PRIVATE "$<$<CONFIG:${CONFIG_VAL}>:${FLAG}>")
    endforeach()
  endfunction()

  if (ANALYZER_EXCLUDE_RELEASE)
    _apply_analyzer_config_exclusions(Release ANALYZER_EXCLUDE_RELEASE)
  endif()

  if (ANALYZER_EXCLUDE_MINSIZEREL)
    _apply_analyzer_config_exclusions(MinSizeRel ANALYZER_EXCLUDE_MINSIZEREL)
  endif()

  patch_compile_commands(REMOVE_FLAGS "-fanalyzer" TARGETS ${TARGET_NAME})

endfunction()
