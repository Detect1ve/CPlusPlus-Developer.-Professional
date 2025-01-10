function(determine_max_compiler_standard)
  if (DEFINED MAX_SUPPORTED_COMPILER_STANDARD)
    return()
  endif()

  set(ALL_CXX_STANDARDS 26 23 20 17 14 11 98)
  foreach(ver IN LISTS ALL_CXX_STANDARDS)
    if ("cxx_std_${ver}" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
      set(MAX_SUPPORTED_COMPILER_STANDARD ${ver} CACHE INTERNAL
        "Maximum C++ standard supported by the compiler")
      message(STATUS "Compiler supports C++${MAX_SUPPORTED_COMPILER_STANDARD} at maximum")
      return()
    endif()
  endforeach()
endfunction()

function(add_subdirectory_if_compatible subdir)
  determine_max_compiler_standard()

  set(SUBDIR_CMAKE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/CMakeLists.txt")
  if (NOT EXISTS ${SUBDIR_CMAKE_FILE})
    message(WARNING "CMakeLists.txt not found for subdirectory ${subdir}")
    return()
  endif()

  file(READ ${SUBDIR_CMAKE_FILE} CMAKE_CONTENT)

  string(REGEX MATCH "(^|\n)[ \t]*set\\(MINIMUM_CXX_STANDARD ([0-9]+)\\)" _ ${CMAKE_CONTENT})
  set(MINIMUM_REQUIRED_STANDARD ${CMAKE_MATCH_2})

  if (MINIMUM_REQUIRED_STANDARD)
    if (MINIMUM_REQUIRED_STANDARD EQUAL 98)
      add_subdirectory(${subdir})
    elseif (MINIMUM_REQUIRED_STANDARD GREATER MAX_SUPPORTED_COMPILER_STANDARD)
      message(WARNING "Skipping project ${subdir}: requires at least "
        "C++${MINIMUM_REQUIRED_STANDARD}, but compiler's maximum is "
        "C++${MAX_SUPPORTED_COMPILER_STANDARD}.")
    else()
      add_subdirectory(${subdir})
    endif()
  else()
    message(WARNING "Could not determine minimum required C++ standard for ${subdir}. "
      "Adding it anyway.")
    add_subdirectory(${subdir})
  endif()
endfunction()

determine_max_compiler_standard()
if (DEFINED MINIMUM_CXX_STANDARD)
  if (NOT MINIMUM_CXX_STANDARD EQUAL 98)
    if (MINIMUM_CXX_STANDARD GREATER MAX_SUPPORTED_COMPILER_STANDARD)
      message(FATAL_ERROR "Project ${PROJECT_NAME} requires at least "
        "C++${MINIMUM_CXX_STANDARD}, but compiler's maximum is "
        "C++${MAX_SUPPORTED_COMPILER_STANDARD}.")
    endif()
  endif()
endif()
set(CMAKE_CXX_STANDARD ${MAX_SUPPORTED_COMPILER_STANDARD})
message(STATUS "Use C++${CMAKE_CXX_STANDARD} standard for ${PROJECT_NAME}")
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
