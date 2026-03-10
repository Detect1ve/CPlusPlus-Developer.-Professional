#ifndef ATTRIBUTE_WRAPPER_HPP
#define ATTRIBUTE_WRAPPER_HPP

#ifdef _MSC_VER
    #define ATTRIBUTE_CONST
    #define ATTRIBUTE_PURE
#else
    #ifdef __has_cpp_attribute
        #if __has_cpp_attribute(gnu::const)
            #define ATTRIBUTE_CONST [[gnu::const]]
        #endif
    #endif

    #ifdef __has_cpp_attribute
        #if __has_cpp_attribute(gnu::pure)
            #define ATTRIBUTE_PURE [[gnu::pure]]
        #endif
    #endif
#endif

#endif // ATTRIBUTE_WRAPPER_HPP
