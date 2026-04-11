include(CheckCXXCompilerFlag)

function(add_flag_if_supported FLAG_LIST_VAR FLAG)
  string(REGEX REPLACE "[^a-zA-Z0-9]" "_" SAFE_FLAG "HAS_FLAG_${FLAG}")

  check_cxx_compiler_flag("${FLAG}" ${SAFE_FLAG})

  if (${SAFE_FLAG})
    set(${FLAG_LIST_VAR} "${${FLAG_LIST_VAR}};${FLAG}" PARENT_SCOPE)
  endif()
endfunction()

function(set_warning_flags TARGET_NAME)
  cmake_parse_arguments(WARNING_FLAGS
    "NO_BASE"
    "SCOPE"
    "EXTRA_CLANG;EXTRA_GCC;EXTRA_MSVC"
    ${ARGN})

  if (NOT WARNING_FLAGS_SCOPE)
    set(WARNING_FLAGS_SCOPE PRIVATE)
  endif()

  if (NOT WARNING_FLAGS_NO_BASE)
    target_compile_options(${TARGET_NAME} ${WARNING_FLAGS_SCOPE} ${COMPILE_WARNING_FLAGS})
  endif()

  set(SUPPORTED_EXTRA_FLAGS "")

  if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    foreach(FLAG IN LISTS WARNING_FLAGS_EXTRA_CLANG)
      add_flag_if_supported(SUPPORTED_EXTRA_FLAGS "${FLAG}")
    endforeach()
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    foreach(FLAG IN LISTS WARNING_FLAGS_EXTRA_GCC)
      add_flag_if_supported(SUPPORTED_EXTRA_FLAGS "${FLAG}")
    endforeach()
  elseif (MSVC)
    foreach(FLAG IN LISTS WARNING_FLAGS_EXTRA_MSVC)
      add_flag_if_supported(SUPPORTED_EXTRA_FLAGS "${FLAG}")
    endforeach()
  endif()

  if (SUPPORTED_EXTRA_FLAGS)
    target_compile_options(${TARGET_NAME} ${WARNING_FLAGS_SCOPE} ${SUPPORTED_EXTRA_FLAGS})
  endif()

  if (NOT WARNING_FLAGS_NO_BASE)
    set_target_properties(${TARGET_NAME} PROPERTIES COMPILE_WARNING_AS_ERROR ON)
  endif()

endfunction()

if (NOT MSVC)
  # Common flags between GCC and Clang.
  set(POTENTIAL_FLAGS -Walloca -Wcast-qual -Wctad-maybe-unsupported -Wdate-time
    -Wdouble-promotion -Wextra-semi -Wfloat-equal -Wformat=2 -Wmissing-include-dirs
    -Wmissing-noreturn -Weffc++ -Wold-style-cast -Wpacked -Wpedantic -Wsuggest-override
    -Wswitch-default -Wswitch-enum -Wundef -Wvla -Wzero-as-null-pointer-constant
    -Wformat-signedness -Wnrvo)

  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # https://gcc.gnu.org/onlinedocs/gcc-15.2.0/gcc/C_002b_002b-Dialect-Options.html
    list(APPEND POTENTIAL_FLAGS
      -fstrict-enums -fvisibility-inlines-hidden -Wctor-dtor-privacy -Winvalid-constexpr
      -Winvalid-imported-macros -Wnoexcept -Wredundant-tags -Wstrict-null-sentinel
      -Woverloaded-virtual -Wsign-promo -Wmismatched-tags -Wmultiple-inheritance
      -Wvirtual-inheritance -Wplacement-new=2 -Wcatch-value=3 -Wconditionally-supported
      -Wsuggest-final-types -Wsuggest-final-methods -Wunused-macros)

    # clang-tidy cannot use it:
    # 1. -fimplicit-constexpr
    # 2. -fnothrow-opt

    # COOL, but need to solve it using cmake:
    # 1. -fvisibility-ms-compat

    # https://gcc.gnu.org/onlinedocs/gcc-15.2.0/gcc/Warning-Options.html
    list(APPEND POTENTIAL_FLAGS
      -Wall -Wextra -Wformat-overflow=2 -Wformat-truncation=2 -Wnull-dereference
      -Wshift-negative-value -Wshift-overflow=2 -Wsync-nand -Wtrivial-auto-var-init
      -Wunused-const-variable=2 -Wuse-after-free=3 -Wuseless-cast -Wstrict-overflow=5
      -Wstringop-overflow=4 -Wsuggest-attribute=pure -Wsuggest-attribute=const
      -Wsuggest-attribute=noreturn -Wsuggest-attribute=malloc -Wsuggest-attribute=format
      -Wmissing-format-attribute -Wsuggest-attribute=cold -Walloc-zero -Warith-conversion
      -Warray-bounds=2 -Wattribute-alias=2 -Wbidi-chars=any -Wduplicated-branches
      -Wduplicated-cond -Wtrampolines -Wshadow -Wunsafe-loop-optimizations
      -Wcast-align=strict -Wconversion -Wenum-conversion -Wsign-conversion -Wlogical-op
      -Wmissing-declarations -Wredundant-decls -Winterference-size -Winvalid-pch
      -Winvalid-utf8 -Wvector-operation-performance -Wdisabled-optimization
      -Wstack-protector -Wflex-array-member-not-at-end -Wtrailing-whitespace=any
      -Wleading-whitespace=spaces)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # https://releases.llvm.org/22.1.0/tools/clang/docs/DiagnosticsReference.html
    list(APPEND POTENTIAL_FLAGS
      -fsafe-buffer-usage-suggestions -Werror -WCL4 -Wabstract-vbase-init
      -Wanon-enum-enum-conversion -Warc-repeated-use-of-weak
      -Warray-bounds-pointer-arithmetic -Wassign-enum -Watomic-implicit-seq-cst
      -Watomic-properties -Wbad-function-cast -Wbinary-literal -Wbind-to-temporary-copy
      -Wc++-compat -Wc++11-extensions -Wc++14-extensions -Wc++17-extensions
      -Wc++20-extensions -Wc++26-extensions -Wcalled-once-parameter -Wcast-align
      -Wcast-function-type -Wclass-varargs -Wcomma -Wcompound-token-split
      -Wconditional-uninitialized -Wconsumed -Wcovered-switch-default
      -Wcstring-format-directive -Wcuda-compat -Wdeclaration-after-statement -Wdeprecated
      -Wdeprecated-implementations -Wdirect-ivar-access -Wdocumentation
      -Wdocumentation-pedantic -Wdtor-name -Wduplicate-enum -Wduplicate-method-arg
      -Wduplicate-method-match -Wdynamic-exception-spec -Wexit-time-destructors
      -Wexpansion-to-defined -Wexplicit-ownership-type -Wextra-semi-stmt -Wformat-non-iso
      -Wformat-pedantic -Wformat-type-confusion -Wfour-char-constants -Wgcc-compat
      -Wglobal-constructors -Wgnu -Wheader-hygiene -Widiomatic-parentheses
      -Wimplicit-fallthrough -Wimplicit-retain-self
      -Wincompatible-function-pointer-types-strict -Wincomplete-module
      -Winconsistent-missing-destructor-override -Winvalid-or-nonexistent-directory
      -Wloop-analysis -Wmain -Wmain-return-type -Wmax-tokens -Wmethod-signatures
      -Wmicrosoft -Wmissing-prototypes -Wmissing-variable-declarations -Rmodule-build
      -Rmodule-import -Rmodule-include-translation -Rmodule-lock -Wnewline-eof -Wnon-gcc
      -Wnonportable-private-system-apinotes-path -Wnonportable-system-include-path
      -Wnullable-to-nonnull-conversion -Wobjc-interface-ivars -Wobjc-messaging-id
      -Wobjc-missing-property-synthesis -Wobjc-property-assign-on-object-type
      -Wobjc-signed-char-bool -Wopenmp -Wover-aligned -Woverriding-method-mismatch
      -Wpadded-bitfield -Wpedantic-core-features -Wpointer-arith
      -Wpoison-system-directories -Wpragmas -Wpre-openmp-51-compat -Wprofile-instr-missing
      -Wquoted-include-in-framework-header -Wreceiver-forward-class -Wredundant-parens
      -Rremark-backend-plugin -Wreserved-identifier -Wreserved-user-defined-literal
      -Rround-trip-cc1-args -Rsanitize-address -Wselector -Wshadow-all
      -Wshift-sign-overflow -Wsigned-enum-bitfield -Wsource-uses-openacc -Wspir-compat
      -Wstatic-in-inline -Wstrict-potentially-direct-selector -Wstrict-prototypes
      -Wstrict-selector-match -Wsuggest-destructor-override -Wsuper-class-method-mismatch
      -Wtautological-constant-in-range-compare -Wthread-safety -Wthread-safety-beta
      -Wthread-safety-negative -Wthread-safety-verbose -Wunaligned-access
      -Wundeclared-selector -Wundef-prefix -Wundefined-func-template
      -Wundefined-reinterpret-cast -Wunguarded-availability -Wunnamed-type-template-args
      -Wunreachable-code-aggressive -Wunsafe-buffer-usage
      -Wunsupported-dll-base-class-template -Wunused-exception-parameter
      -Wunused-member-function -Wunused-template -Wused-but-marked-unused
      -Wvariadic-macros -Wvector-conversion -Wweak-template-vtables -Wweak-vtables
      -Wdecls-in-multiple-modules -Wfunction-effects -Wnvcc-compat
      -Wexperimental-lifetime-safety -Wexperimental-lifetime-safety-suggestions
      -Wfunction-effect-redeclarations -Whlsl-implicit-binding
      -Wignored-base-class-qualifiers -Wmodule-file-mapping-mismatch -Rmodule-map
      -Rmodule-validation -Wms-bitfield-padding -Wshadow-header -Wshift-bool
      -Wthread-safety-pointer -Wunique-object-duplication)
    # -Rpass
    # -Rpass-analysis
    # -Rpass-missed
    # -Rsearch-path-usage
  endif()
else()
  set(POTENTIAL_FLAGS
    # https://learn.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-by-category?view=msvc-170#language
    /Zc:__cplusplus /Zc:inline /Zc:preprocessor /Zc:templateScope /Zc:throwingNew
    # https://learn.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-by-category?view=msvc-170#diagnostics
    /sdl
    # /W4
    # /w14242 /w14296 /w14355 /w14625 /w14626 /w15026 /w15027 /w15038 /w15246
    /Wall /wd4365 /wd4514 /wd4668 /wd4710 /wd4711 /wd4820 /wd4865 /wd5039 /wd5045
    /WX)
endif()

foreach(FLAG IN LISTS POTENTIAL_FLAGS)
  add_flag_if_supported(COMPILE_WARNING_FLAGS "${FLAG}")
endforeach()
