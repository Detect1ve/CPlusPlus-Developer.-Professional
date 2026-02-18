function(patch_compile_commands)
  cmake_parse_arguments(PATCH
    ""
    ""
    "REMOVE_FLAGS;TARGETS"
    ${ARGN})

  if (NOT PATCH_REMOVE_FLAGS)
    return()
  endif()

  if (NOT TARGET patch_compile_commands)
    add_custom_target(patch_compile_commands ALL
      DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
      COMMENT "Patching compile_commands.json")
    set_target_properties(patch_compile_commands PROPERTIES PATCHED_FLAGS "")
  endif()

  get_target_property(ALREADY_PATCHED patch_compile_commands PATCHED_FLAGS)

  foreach(FLAG IN LISTS PATCH_REMOVE_FLAGS)
    if (NOT "${FLAG}" IN_LIST ALREADY_PATCHED)
      add_custom_command(TARGET patch_compile_commands POST_BUILD
        COMMAND sed -i "s/${FLAG}//g" ${CMAKE_BINARY_DIR}/compile_commands.json
        VERBATIM
        COMMENT "Removing ${FLAG} from compile_commands.json")
      list(APPEND ALREADY_PATCHED "${FLAG}")
    endif()
  endforeach()

  set_target_properties(patch_compile_commands PROPERTIES PATCHED_FLAGS "${ALREADY_PATCHED}")

  foreach(T IN LISTS PATCH_TARGETS)
    if (TARGET "${T}")
      add_dependencies("${T}" patch_compile_commands)
    endif()
  endforeach()
endfunction()
