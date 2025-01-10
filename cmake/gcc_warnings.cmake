if (NOT MSVC)
  # Common flags between GCC and Clang.
  set(COMPILE_WARNING_FLAGS -Werror=alloca -Werror=cast-qual
    -Werror=ctad-maybe-unsupported -Werror=date-time -Werror=double-promotion
    -Werror=extra-semi -Werror=float-equal -Werror=format=2 -Werror=missing-include-dirs
    -Werror=missing-noreturn -Werror=effc++ -Werror=old-style-cast -Werror=packed
    -Werror=pedantic -Werror=suggest-override -Werror=switch-default -Werror=switch-enum
    -Werror=undef -Werror=vla -Werror=zero-as-null-pointer-constant)

  if (  CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
     OR (  CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
        AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19))
    set(COMPILE_WARNING_FLAGS ${COMPILE_WARNING_FLAGS}
      -Werror=format-signedness)
  endif()

  if (  (  CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
       AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 14)
    OR  ( CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
       AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19))
    set(COMPILE_WARNING_FLAGS ${COMPILE_WARNING_FLAGS}
      -Werror=nrvo)
  endif()

  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # https://gcc.gnu.org/onlinedocs/gcc-15.2.0/gcc/C_002b_002b-Dialect-Options.html
    set(COMPILE_WARNING_FLAGS ${COMPILE_WARNING_FLAGS}
      -fstrict-enums -fvisibility-inlines-hidden -Werror=ctor-dtor-privacy
      -Werror=invalid-constexpr -Werror=invalid-imported-macros -Werror=noexcept
      -Werror=redundant-tags -Werror=strict-null-sentinel -Werror=overloaded-virtual
      -Werror=sign-promo -Werror=mismatched-tags -Werror=multiple-inheritance
      -Werror=virtual-inheritance -Werror=placement-new=2 -Werror=catch-value=3
      -Werror=conditionally-supported -Werror=suggest-final-types
      -Werror=suggest-final-methods -Werror=unused-macros)

    # clang-tidy cannot use it:
    # 1. -fimplicit-constexpr
    # 2. -fnothrow-opt

    # COOL, but need to solve it using cmake:
    # 1. -fvisibility-ms-compat

    # https://gcc.gnu.org/onlinedocs/gcc-15.2.0/gcc/Warning-Options.html
    set(COMPILE_WARNING_FLAGS ${COMPILE_WARNING_FLAGS}
      -Werror=all -Werror=extra -Werror=format-overflow=2 -Werror=format-truncation=2
      -Werror=null-dereference -Werror=shift-negative-value -Werror=shift-overflow=2
      -Werror=sync-nand -Werror=trivial-auto-var-init -Werror=unused-const-variable=2
      -Werror=use-after-free=3 -Werror=useless-cast -Werror=strict-overflow=5
      -Werror=stringop-overflow=4 -Werror=suggest-attribute=pure
      -Werror=suggest-attribute=const -Werror=suggest-attribute=noreturn
      -Werror=suggest-attribute=malloc -Werror=suggest-attribute=format
      -Werror=missing-format-attribute -Werror=suggest-attribute=cold -Werror=alloc-zero
      -Werror=arith-conversion -Werror=array-bounds=2 -Werror=attribute-alias=2
      -Werror=bidi-chars=any -Werror=duplicated-branches -Werror=duplicated-cond
      -Werror=trampolines -Werror=shadow -Werror=unsafe-loop-optimizations
      -Werror=cast-align=strict -Werror=conversion -Werror=enum-conversion
      -Werror=sign-conversion -Werror=logical-op -Werror=missing-declarations
      -Werror=redundant-decls -Werror=interference-size -Werror=invalid-pch
      -Werror=invalid-utf8 -Werror=long-long -Werror=vector-operation-performance
      -Werror=disabled-optimization -Werror=stack-protector
      )

    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 14)
      set(COMPILE_WARNING_FLAGS ${COMPILE_WARNING_FLAGS}
        -Werror=flex-array-member-not-at-end)
    endif()

    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 15)
      set(COMPILE_WARNING_FLAGS ${COMPILE_WARNING_FLAGS}
        -Werror=trailing-whitespace=any -Werror=leading-whitespace=spaces)
    endif()
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  # https://releases.llvm.org/21.1.0/tools/clang/docs/DiagnosticsReference.html
    set(COMPILE_WARNING_FLAGS ${COMPILE_WARNING_FLAGS}
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
      -Wvariadic-macros -Wvector-conversion -Wweak-template-vtables -Wweak-vtables)
    # -Rpass
    # -Rpass-analysis
    # -Rpass-missed
    # -Rsearch-path-usage
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19)
      set(COMPILE_WARNING_FLAGS ${COMPILE_WARNING_FLAGS}
        -Wdecls-in-multiple-modules -Wexperimental-lifetime-safety -Wfunction-effects
        -Whlsl-implicit-binding -Wignored-base-class-qualifiers
        -Wmodule-file-mapping-mismatch -Rmodule-map -Wms-bitfield-padding -Wnvcc-compat
        -Wshift-bool -Wthread-safety-pointer -Wunique-object-duplication)
    endif()
  endif()
else()
  set(COMPILE_WARNING_FLAGS /W4)
endif()
