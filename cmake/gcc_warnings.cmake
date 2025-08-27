if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # https://gcc.gnu.org/onlinedocs/gcc-15.2.0/gcc/C_002b_002b-Dialect-Options.html
  set(GCC_FLAGS -fstrict-enums -fvisibility-inlines-hidden -Werror=ctad-maybe-unsupported
    -Werror=ctor-dtor-privacy -Werror=invalid-constexpr -Werror=invalid-imported-macros
    -Werror=noexcept -Werror=redundant-tags -Werror=effc++ -Werror=strict-null-sentinel
    -Werror=old-style-cast -Werror=overloaded-virtual -Werror=sign-promo
    -Werror=mismatched-tags -Werror=multiple-inheritance -Werror=virtual-inheritance
    -Werror=placement-new=2 -Werror=catch-value=3 -Werror=conditionally-supported
    -Werror=extra-semi -Werror=suggest-final-types -Werror=suggest-final-methods
    -Werror=suggest-override)

  # clang-tidy cannot use it:
  # 1. -fimplicit-constexpr
  # 2. -fnothrow-opt

  # COOL, but need to solve it using cmake:
  # 1. -fvisibility-ms-compat

  # https://gcc.gnu.org/onlinedocs/gcc-15.2.0/gcc/Warning-Options.html
  set(GCC_FLAGS ${GCC_FLAGS}
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
    set(GCC_FLAGS ${GCC_FLAGS}
      -Werror=nrvo -Werror=flex-array-member-not-at-end)
  endif()

  if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 15)
    set(GCC_FLAGS ${GCC_FLAGS}
      -Werror=trailing-whitespace=any -Werror=leading-whitespace=spaces)
  endif()
endif()