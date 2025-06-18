#ifndef MODEL_PRIMITIVE_HPP
#define MODEL_PRIMITIVE_HPP

#include <memory>
#if defined(_MSC_VER) && !defined(__clang__) && !defined(__INTEL_COMPILER)
#include <string>
#endif

struct Point
{
    int x;
    int y;
};

namespace editor::model
{
    class Primitive
    {
    public:
        virtual ~Primitive() = default;

        Primitive() = default;
        Primitive(const Primitive&) = delete;
        Primitive& operator=(const Primitive&) = delete;
        Primitive(Primitive&&) = delete;
        Primitive& operator=(Primitive&&) = delete;

        [[nodiscard]] virtual std::string getType() const  = 0;
    };
} // namespace editor::model

#endif /* MODEL_PRIMITIVE_HPP */
