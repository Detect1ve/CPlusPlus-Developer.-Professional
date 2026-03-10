include_guard(GLOBAL)

include(CheckIPOSupported)

check_ipo_supported(
  RESULT _LTO_SUPPORTED
  OUTPUT _LTO_ERROR
  LANGUAGES CXX)

if (NOT _LTO_SUPPORTED)
  message(STATUS "LTO is not supported: ${_LTO_ERROR}")
endif()

function(enable_lto)
  if (NOT _LTO_SUPPORTED)
    return()
  endif()

  foreach(target IN LISTS ARGN)
    if (TARGET ${target})
      message(STATUS "enable_lto: for ${target} target")

      set_target_properties(${target} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    else()
      message(WARNING "enable_lto: target '${target}' does not exist")
    endif()
  endforeach()
endfunction()
