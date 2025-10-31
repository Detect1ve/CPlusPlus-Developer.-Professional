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
# -W/-Wextra
# https://releases.llvm.org/21.1.0/tools/clang/docs/DiagnosticsReference.html
  set(COMPILE_WARNING_FLAGS
    -fsafe-buffer-usage-suggestions
    -Werror
    # -W
    -Wabstract-vbase-init
    -Wall
    -Walloca
    -Wanon-enum-enum-conversion
    -Warc-maybe-repeated-use-of-weak
    -Warc-repeated-use-of-weak
    -Warray-bounds-pointer-arithmetic
    -Warray-parameter
    -Wassign-enum
    -Watomic-implicit-seq-cst
    -Watomic-properties
    -Wauto-decl-extensions
    -Wbad-function-cast
    -Wbinary-literal
    # -Wbind-to-temporary-copy # Here for now
    -Wbit-int-extension
    -Wbitfield-enum-conversion
    -Wbitwise-instead-of-logical
    -Wbitwise-op-parentheses
    -Wbool-operation
    -Wc++-compat
    -Wc++-hidden-decl
    -Wc++-keyword
    -Wc++-unterminated-string-initialization
    # -Wc++11-compat
    # -Wc++11-compat-pedantic
    -Wc++11-compat-reserved-user-defined-literal
    -Wc++11-extensions
    -Wc++11-extra-semi
    -Wc++11-long-long
    -Wc++11-narrowing
    -Wc++14-attribute-extensions
    -Wc++14-binary-literal
    # -Wc++14-compat
    # -Wc++14-compat-pedantic
    -Wc++14-extensions
    -Wc++17-attribute-extensions
    # -Wc++17-compat
    # -Wc++17-compat-pedantic
    -Wc++17-extensions
    -Wc++20-attribute-extensions
    -Wc++20-compat
    -Wc++20-compat-pedantic
    -Wc++20-designator
    -Wc++20-extensions
    -Wc++23-attribute-extensions
    -Wc++23-compat
    -Wc++26-extensions
    -Wc++98-c++11-compat-binary-literal
    -Wc++98-compat-extra-semi
    # -Wc++98-compat-local-type-template-args
    -Wc++98-compat-unnamed-type-template-args
    -Wcalled-once-parameter
    -Wcast-align
    -Wcast-function-type
    -Wcast-function-type-mismatch
    -Wcast-function-type-strict
    -Wcast-qual
    -Wchar-subscripts
    -Wclass-varargs
    -Wcomma
    -Wcomment
    -Wcompletion-handler
    -Wcomplex-component-init
    -Wcompound-token-split
    -Wcompound-token-split-by-space
    -Wconditional-uninitialized
    -Wconsumed
    -Wconversion
    -Wcovered-switch-default
    -Wcstring-format-directive
    -Wctad-maybe-unsupported
    -Wcuda-compat
    -Wcustom-atomic-properties
    -Wdate-time
    -Wdeclaration-after-statement
    -Wdecls-in-multiple-modules
    -Wdefault-const-init
    -Wdefault-const-init-field
    -Wdefault-const-init-var
    -Wdelete-non-abstract-non-virtual-dtor
    -Wdelete-non-virtual-dtor
    -Wdelimited-escape-sequence-extension
    -Wdeprecated
    -Wdeprecated-copy
    -Wdeprecated-copy-with-dtor
    -Wdeprecated-copy-with-user-provided-copy
    -Wdeprecated-copy-with-user-provided-dtor
    -Wdeprecated-dynamic-exception-spec
    -Wdeprecated-implementations
    -Wdeprecated-redundant-constexpr-static-def
    -Wdirect-ivar-access
    # -Wdisabled-macro-expansion
    -Wdocumentation
    -Wdocumentation-deprecated-sync
    -Wdocumentation-html
    -Wdocumentation-pedantic
    -Wdocumentation-unknown-command
    -Wdollar-in-identifier-extension
    -Wdouble-promotion
    -Wdtor-name
    -Wduplicate-decl-specifier
    -Wduplicate-enum
    -Wduplicate-method-arg
    -Wduplicate-method-match
    -Wdynamic-exception-spec
    -Wembedded-directive
    -Wempty-init-stmt
    -Wempty-translation-unit
    -Wenum-compare-conditional
    -Wenum-conversion
    -Wenum-enum-conversion
    -Wenum-float-conversion
    -Wexit-time-destructors # debug
    -Wexpansion-to-defined
    -Wexperimental-lifetime-safety
    -Wexplicit-ownership-type
    -Wextra
    -Wextra-semi
    -Wextra-semi-stmt
    -Wflexible-array-extensions
    -Wfloat-conversion
    -Wfloat-equal
    -Wfloat-overflow-conversion
    -Wfloat-zero-conversion
    -Wfor-loop-analysis
    -Wformat-non-iso
    -Wformat-nonliteral
    -Wformat-pedantic
    -Wformat-signedness
    -Wformat-type-confusion
    -Wformat=2
    -Wfour-char-constants
    -Wframe-address
    -Wfunction-effects
    -Wfuse-ld-path
    -Wfuture-attribute-extensions
    -Wgcc-compat
    -Wglobal-constructors
    -Wgnu
    -Wgnu-anonymous-struct
    -Wgnu-auto-type
    -Wgnu-case-range
    -Wgnu-complex-integer
    -Wgnu-compound-literal-initializer
    -Wgnu-conditional-omitted-operand
    -Wgnu-designator
    -Wgnu-empty-struct
    -Wgnu-flexible-array-initializer
    -Wgnu-flexible-array-union-member
    -Wgnu-folding-constant
    -Wgnu-imaginary-constant
    -Wgnu-include-next
    -Wgnu-label-as-value
    -Wgnu-line-marker
    -Wgnu-null-pointer-arithmetic
    -Wgnu-pointer-arith
    -Wgnu-redeclared-enum
    -Wgnu-statement-expression
    -Wgnu-statement-expression-from-macro-expansion
    -Wgnu-union-cast
    -Wgnu-zero-line-directive
    -Wgnu-zero-variadic-macro-arguments
    -Wheader-hygiene
    -Whlsl-implicit-binding
    -Widiomatic-parentheses
    -Wignored-base-class-qualifiers
    -Wignored-qualifiers
    -Wimplicit
    -Wimplicit-atomic-properties
    -Wimplicit-fallthrough
    -Wimplicit-fallthrough-per-function
    -Wimplicit-float-conversion
    -Wimplicit-function-declaration
    -Wimplicit-int
    -Wimplicit-int-conversion
    -Wimplicit-int-conversion-on-negation
    -Wimplicit-int-enum-cast
    -Wimplicit-int-float-conversion
    -Wimplicit-retain-self
    -Wimplicit-void-ptr-cast
    -Wimport-preprocessor-directive-pedantic
    -Wincompatible-function-pointer-types-strict
    -Wincomplete-module
    -Winconsistent-missing-destructor-override
    -Winfinite-recursion
    -Wint-in-bool-context
    -Winvalid-or-nonexistent-directory
    -Winvalid-utf8
    -Wjump-misses-init
    -Wkeyword-macro
    -Wlanguage-extension-token
    # -Wlocal-type-template-args # useless
    -Wlogical-op-parentheses
    -Wlong-long
    -Wloop-analysis
    -Wmain
    -Wmain-return-type
    -Wmax-tokens
    -Wmethod-signatures
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
    -Wmisleading-indentation
    -Wmismatched-tags
    -Wmissing-braces
    -Wmissing-designated-field-initializers
    -Wmissing-field-initializers
    -Wmissing-include-dirs
    -Wmissing-method-return-type
    -Wmissing-noreturn
    -Wmissing-prototypes
    -Wmissing-variable-declarations
    -Rmodule-build
    -Wmodule-file-mapping-mismatch
    -Rmodule-import
    -Rmodule-include-translation
    -Rmodule-lock
    -Rmodule-map
    -Wmost
    -Wmove
    -Wms-bitfield-padding
    -Wnested-anon-types
    -Wnewline-eof
    -Wnon-gcc
    -Wnon-modular-include-in-framework-module
    -Wnon-modular-include-in-module
    -Wnon-virtual-dtor
    -Wnonportable-private-system-apinotes-path
    -Wnonportable-system-include-path
    -Wnrvo
    -Wnull-pointer-arithmetic
    -Wnull-pointer-subtraction
    -Wnullability-extension
    -Wnullable-to-nonnull-conversion
    -Wnvcc-compat
    -Wobjc-interface-ivars
    -Wobjc-messaging-id
    -Wobjc-missing-property-synthesis
    -Wobjc-property-assign-on-object-type
    -Wobjc-signed-char-bool
    -Wobjc-signed-char-bool-implicit-int-conversion
    -Woctal-prefix-extension
    -Wold-style-cast
    -Wopenmp
    -Wover-aligned
    -Woverlength-strings
    -Woverloaded-virtual
    -Woverriding-method-mismatch
    -Wpacked
    -Wpacked-non-pod
    # -Wpadded
    -Wpadded-bitfield
    -Wparentheses
    # -Rpass
    # -Rpass-analysis
    # -Rpass-missed
    -Wpedantic
    -Wpedantic-core-features
    -Wperf-constraint-implies-noexcept
    -Wpessimizing-move
    -Wpointer-arith
    -Wpoison-system-directories
    -Wpragma-pack
    -Wpragma-pack-suspicious-include
    -Wpragmas
    # -Wpre-c++14-compat
    # -Wpre-c++14-compat-pedantic
    # -Wpre-c++17-compat
    # -Wpre-c++17-compat-pedantic
    # -Wpre-c++20-compat
    # -Wpre-c++20-compat-pedantic
    -Wpre-c++23-compat
    -Wpre-c++23-compat-pedantic
    -Wpre-c++26-compat
    -Wpre-openmp-51-compat
    -Wprofile-instr-missing
    -Wquoted-include-in-framework-header
    -Wrange-loop-analysis
    -Wrange-loop-bind-reference
    -Wrange-loop-construct
    -Wreceiver-forward-class
    -Wredundant-move
    -Wredundant-parens
    -Rremark-backend-plugin
    -Wreorder
    -Wreorder-ctor
    -Wreserved-attribute-identifier
    -Wreserved-identifier
    -Wreserved-macro-identifier
    -Wreserved-user-defined-literal
    -Wretained-language-linkage
    -Rround-trip-cc1-args
    -Rsanitize-address
    # -Rsearch-path-usage
    -Wselector
    -Wselector-type-mismatch
    -Wself-assign
    -Wself-assign-overloaded
    -Wself-move
    -Wsemicolon-before-method-body
    -Wshadow
    -Wshadow-all
    -Wshadow-field
    -Wshadow-field-in-constructor
    -Wshadow-field-in-constructor-modified
    -Wshadow-uncaptured-local
    -Wshift-bool
    -Wshift-sign-overflow
    -Wshorten-64-to-32
    -Wsign-compare
    -Wsign-conversion
    -Wsigned-enum-bitfield
    -Wsometimes-uninitialized
    -Wsource-uses-openacc
    -Wsource-uses-openmp
    -Wspir-compat
    -Wstatic-in-inline
    -Wstrict-potentially-direct-selector
    -Wstrict-prototypes
    -Wstrict-selector-match
    -Wstring-concatenation
    -Wstring-conversion
    -Wsuggest-destructor-override
    -Wsuggest-override
    -Wsuper-class-method-mismatch
    -Wswitch-default
    -Wswitch-enum
    -Wtautological-bitwise-compare
    -Wtautological-compare
    -Wtautological-constant-in-range-compare
    -Wtautological-negation-compare
    -Wtautological-overlap-compare
    -Wtautological-type-limit-compare
    -Wtautological-unsigned-char-zero-compare
    -Wtautological-unsigned-enum-zero-compare
    -Wtautological-unsigned-zero-compare
    -Wtautological-value-range-compare
    -Wtentative-definition-compat
    -Wtentative-definition-incomplete-type
    -Wthread-safety
    -Wthread-safety-analysis
    -Wthread-safety-attributes
    -Wthread-safety-beta
    -Wthread-safety-negative
    -Wthread-safety-pointer
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
    -Wuninitialized
    -Wuninitialized-const-pointer
    -Wuninitialized-const-reference
    -Wunique-object-duplication
    -Wunknown-pragmas
    -Wunnamed-type-template-args
    -Wunnecessary-virtual-specifier
    -Wunneeded-internal-declaration
    -Wunneeded-member-function
    -Wunreachable-code
    -Wunreachable-code-aggressive
    -Wunreachable-code-break
    -Wunreachable-code-fallthrough
    -Wunreachable-code-loop-increment
    -Wunreachable-code-return
    -Wunsafe-buffer-usage
    -Wunsafe-buffer-usage-in-container
    -Wunsafe-buffer-usage-in-libc-call
    -Wunsupported-dll-base-class-template
    -Wunterminated-string-initialization
    -Wunused
    -Wunused-but-set-parameter
    -Wunused-but-set-variable
    -Wunused-const-variable
    -Wunused-exception-parameter
    -Wunused-function
    -Wunused-label
    -Wunused-lambda-capture
    -Wunused-local-typedef
    -Wunused-macros
    -Wunused-member-function
    -Wunused-parameter
    -Wunused-private-field
    -Wunused-property-ivar
    -Wunused-template
    -Wunused-variable
    -Wused-but-marked-unused
    -Wvariadic-macro-arguments-omitted
    -Wvariadic-macros
    -Wvector-conversion
    -Wvla
    -Wvla-cxx-extension
    -Wvla-extension
    -Wvla-extension-static-assert
    -Wweak-template-vtables
    -Wweak-vtables
    -Wzero-as-null-pointer-constant
    -Wzero-length-array
    )
endif()
