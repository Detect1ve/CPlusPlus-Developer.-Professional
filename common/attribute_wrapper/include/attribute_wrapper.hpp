#ifndef ATTRIBUTE_WRAPPER_HPP
#define ATTRIBUTE_WRAPPER_HPP

#ifdef __CPPCHECK__ // FIX syntaxError
    #define __has_cpp_attribute(x) 0
#endif

#ifdef _MSC_VER
    #define ATTRIBUTE_CONST
    #define ATTRIBUTE_PURE
#else
    #ifdef __has_cpp_attribute
        #if __has_cpp_attribute(gnu::const)
            #define ATTRIBUTE_CONST [[gnu::const]]
        #endif

        #if __has_cpp_attribute(gnu::pure)
            #define ATTRIBUTE_PURE [[gnu::pure]]
        #endif
    #endif
#endif

#endif // ATTRIBUTE_WRAPPER_HPP
