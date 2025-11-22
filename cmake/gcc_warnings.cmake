if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # https://gcc.gnu.org/onlinedocs/gcc-15.2.0/gcc/C_002b_002b-Dialect-Options.html
  set(COMPILE_WARNING_FLAGS -fstrict-enums -fvisibility-inlines-hidden
    -Werror=ctad-maybe-unsupported -Werror=ctor-dtor-privacy -Werror=invalid-constexpr
    -Werror=invalid-imported-macros -Werror=noexcept -Werror=redundant-tags -Werror=effc++
    -Werror=strict-null-sentinel -Werror=old-style-cast -Werror=overloaded-virtual
    -Werror=sign-promo -Werror=mismatched-tags -Werror=multiple-inheritance
    -Werror=virtual-inheritance -Werror=placement-new=2 -Werror=catch-value=3
    -Werror=conditionally-supported -Werror=extra-semi -Werror=suggest-final-types
    -Werror=suggest-final-methods -Werror=suggest-override)

  # clang-tidy cannot use it:
  # 1. -fimplicit-constexpr
  # 2. -fnothrow-opt

  # COOL, but need to solve it using cmake:
  # 1. -fvisibility-ms-compat

  # https://gcc.gnu.org/onlinedocs/gcc-15.2.0/gcc/Warning-Options.html
  set(COMPILE_WARNING_FLAGS ${COMPILE_WARNING_FLAGS}
    -Werror=pedantic -Werror=all -Werror=extra -Werror=double-promotion -Werror=format=2
    -Werror=format-overflow=2 -Werror=format-signedness -Werror=format-truncation=2
    -Werror=null-dereference -Werror=missing-include-dirs -Werror=shift-negative-value
    -Werror=shift-overflow=2 -Werror=switch-default -Werror=switch-enum -Werror=sync-nand
    -Werror=trivial-auto-var-init -Werror=unused-const-variable=2 -Werror=use-after-free=3
    -Werror=useless-cast -Werror=strict-overflow=5 -Werror=stringop-overflow=4
    -Werror=suggest-attribute=pure -Werror=suggest-attribute=const
    -Werror=suggest-attribute=noreturn -Werror=missing-noreturn
    -Werror=suggest-attribute=malloc -Werror=suggest-attribute=format
    -Werror=missing-format-attribute -Werror=suggest-attribute=cold -Werror=alloc-zero
    -Werror=alloca -Werror=arith-conversion -Werror=array-bounds=2
    -Werror=attribute-alias=2 -Werror=bidi-chars=any -Werror=duplicated-branches
    -Werror=duplicated-cond -Werror=zero-as-null-pointer-constant -Werror=trampolines
    -Werror=float-equal -Werror=shadow -Werror=unsafe-loop-optimizations -Werror=undef
    -Werror=unused-macros -Werror=cast-qual -Werror=cast-align=strict -Werror=conversion
    -Werror=date-time -Werror=enum-conversion -Werror=sign-conversion -Werror=logical-op
    -Werror=missing-declarations -Werror=packed -Werror=redundant-decls
    -Werror=interference-size -Werror=invalid-pch -Werror=invalid-utf8 -Werror=long-long
    -Werror=vector-operation-performance -Werror=vla -Werror=disabled-optimization
    -Werror=stack-protector)

  if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 14)
    set(COMPILE_WARNING_FLAGS ${COMPILE_WARNING_FLAGS}
      -Werror=nrvo -Werror=flex-array-member-not-at-end)
  endif()

  if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 15)
    set(COMPILE_WARNING_FLAGS ${COMPILE_WARNING_FLAGS}
      -Werror=trailing-whitespace=any -Werror=leading-whitespace=spaces)
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
# https://releases.llvm.org/21.1.0/tools/clang/docs/DiagnosticsReference.html
  set(COMPILE_WARNING_FLAGS
    -fsafe-buffer-usage-suggestions
    -Werror
    -Wabstract-vbase-init
    -Wall
    -Walloca
    -Wanon-enum-enum-conversion
    -Warc-repeated-use-of-weak
    -Warray-bounds-pointer-arithmetic
    -Wassign-enum
    -Watomic-implicit-seq-cst
    -Watomic-properties
    -Wauto-decl-extensions
    -Wbad-function-cast
    -Wbinary-literal
    -Wbind-to-temporary-copy
    -Wbit-int-extension
    -Wc++-compat
    -Wc++11-extensions
    -Wc++14-extensions
    -Wc++17-extensions
    -Wc++20-extensions
    -Wc++26-extensions
    -Wc++98-compat-unnamed-type-template-args
    -Wcalled-once-parameter
    -Wcast-align
    -Wcast-function-type
    -Wcast-qual
    -Wclass-varargs
    -Wcomma
    -Wcomplex-component-init
    -Wcompound-token-split
    -Wconditional-uninitialized
    -Wconsumed
    -Wconversion
    -Wcovered-switch-default
    -Wcstring-format-directive
    -Wctad-maybe-unsupported
    -Wcuda-compat
    -Wdate-time
    -Wdeclaration-after-statement
    -Wdelimited-escape-sequence-extension
    -Wdeprecated
    -Wdeprecated-implementations
    -Wdirect-ivar-access
    -Wdocumentation
    -Wdocumentation-pedantic
    -Wdollar-in-identifier-extension
    -Wdouble-promotion
    -Wdtor-name
    -Wduplicate-enum
    -Wduplicate-method-arg
    -Wduplicate-method-match
    -Wdynamic-exception-spec
    -Wembedded-directive
    -Wempty-translation-unit
    -Wexit-time-destructors
    -Wexpansion-to-defined
    -Wexplicit-ownership-type
    -Wextra
    -Wextra-semi
    -Wextra-semi-stmt
    -Wflexible-array-extensions
    -Wfloat-equal
    -Wformat-non-iso
    -Wformat-pedantic
    -Wformat-type-confusion
    -Wformat=2
    -Wfour-char-constants
    -Wfuture-attribute-extensions
    -Wgcc-compat
    -Wglobal-constructors
    -Wgnu
    -Wheader-hygiene
    -Widiomatic-parentheses
    -Wimplicit-fallthrough
    -Wimplicit-retain-self
    -Wimport-preprocessor-directive-pedantic
    -Wincompatible-function-pointer-types-strict
    -Wincomplete-module
    -Winconsistent-missing-destructor-override
    -Winvalid-or-nonexistent-directory
    -Winvalid-utf8
    -Wkeyword-macro
    -Wlanguage-extension-token
    -Wlong-long
    -Wloop-analysis
    -Wmain
    -Wmain-return-type
    -Wmax-tokens
    -Wmethod-signatures # Check here!
    -Wmicrosoft
    -Wmicrosoft-anon-tag
    -Wmicrosoft-charize
    -Wmicrosoft-comment-paste
    -Wmicrosoft-cpp-macro
    -Wmicrosoft-end-of-file
    -Wmicrosoft-enum-value
    -Wmicrosoft-exception-spec
    -Wmicrosoft-fixed-enum
    -Wmicrosoft-flexible-array
    -Wmicrosoft-redeclare-static
    -Wmissing-include-dirs
    -Wmissing-noreturn
    -Wmissing-prototypes
    -Wmissing-variable-declarations
    -Rmodule-build
    -Rmodule-import
    -Rmodule-include-translation
    -Rmodule-lock
    -Wnested-anon-types
    -Wnewline-eof
    -Wnon-gcc
    -Wnon-virtual-dtor
    -Wnonportable-private-system-apinotes-path
    -Wnonportable-system-include-path
    -Wnullability-extension
    -Wnullable-to-nonnull-conversion
    -Wobjc-interface-ivars
    -Wobjc-messaging-id
    -Wobjc-missing-property-synthesis
    -Wobjc-property-assign-on-object-type
    -Wobjc-signed-char-bool
    -Wold-style-cast
    -Wopenmp
    -Wover-aligned
    -Woverlength-strings
    -Woverriding-method-mismatch
    -Wpacked
    # -Wpadded
    -Wpadded-bitfield
    # -Rpass
    # -Rpass-analysis
    # -Rpass-missed
    -Wpedantic
    -Wpedantic-core-features
    -Wpointer-arith
    -Wpoison-system-directories
    -Wpragma-pack
    -Wpragma-pack-suspicious-include
    -Wpragmas
    -Wpre-openmp-51-compat
    -Wprofile-instr-missing
    -Wquoted-include-in-framework-header
    -Wreceiver-forward-class
    -Wredundant-parens
    -Rremark-backend-plugin
    -Wreserved-identifier
    -Wreserved-macro-identifier
    -Wreserved-user-defined-literal
    -Wretained-language-linkage
    -Rround-trip-cc1-args
    -Rsanitize-address
    # -Rsearch-path-usage
    -Wselector
    -Wselector-type-mismatch
    -Wshadow
    -Wshadow-all
    -Wshadow-field
    -Wshadow-field-in-constructor
    -Wshadow-field-in-constructor-modified
    -Wshadow-uncaptured-local
    -Wshift-sign-overflow
    -Wsigned-enum-bitfield
    -Wsource-uses-openacc
    -Wsource-uses-openmp
    -Wspir-compat
    -Wstatic-in-inline
    -Wstrict-potentially-direct-selector
    -Wstrict-prototypes
    -Wstrict-selector-match
    -Wsuggest-destructor-override
    -Wsuggest-override
    -Wsuper-class-method-mismatch
    -Wswitch-default
    -Wswitch-enum
    -Wtautological-constant-in-range-compare
    -Wtautological-type-limit-compare
    -Wtautological-unsigned-char-zero-compare
    -Wtautological-unsigned-enum-zero-compare
    -Wtautological-unsigned-zero-compare
    -Wtautological-value-range-compare
    -Wtentative-definition-incomplete-type
    -Wthread-safety
    -Wthread-safety-analysis
    -Wthread-safety-attributes
    -Wthread-safety-beta
    -Wthread-safety-negative
    -Wthread-safety-precise
    -Wthread-safety-reference
    -Wthread-safety-reference-return
    -Wthread-safety-verbose
    -Wtype-limits
    -Wunaligned-access
    -Wundeclared-selector
    -Wundef
    -Wundef-prefix
    -Wundefined-func-template
    -Wundefined-internal-type
    -Wundefined-reinterpret-cast
    -Wunguarded-availability
    -Wunnamed-type-template-args
    -Wunneeded-member-function
    -Wunreachable-code
    -Wunreachable-code-aggressive
    -Wunreachable-code-break
    -Wunreachable-code-fallthrough
    -Wunreachable-code-loop-increment
    -Wunreachable-code-return
    -Wunsafe-buffer-usage
    -Wunsafe-buffer-usage-in-container
    -Wunsupported-dll-base-class-template
    -Wunused-exception-parameter
    -Wunused-macros
    -Wunused-member-function
    -Wunused-template
    -Wused-but-marked-unused
    -Wvariadic-macros
    -Wvector-conversion
    -Wvla
    -Wweak-template-vtables
    -Wweak-vtables
    -Wzero-as-null-pointer-constant
    )
  if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19)
    set(COMPILE_WARNING_FLAGS ${COMPILE_WARNING_FLAGS}
      -Wc++23-compat
      -Wdecls-in-multiple-modules
      -Wexperimental-lifetime-safety
      -Wformat-signedness
      -Wfunction-effects
      -Whlsl-implicit-binding
      -Wignored-base-class-qualifiers
      -Wmodule-file-mapping-mismatch
      -Rmodule-map
      -Wms-bitfield-padding
      -Wnrvo
      -Wnvcc-compat
      -Woctal-prefix-extension
      -Wreserved-attribute-identifier
      -Wshift-bool
      -Wthread-safety-pointer
      -Wunique-object-duplication
      -Wunsafe-buffer-usage-in-libc-call
      )
  endif()
elseif(MSVC)
  set(COMPILE_WARNING_FLAGS /W4)
endif()
