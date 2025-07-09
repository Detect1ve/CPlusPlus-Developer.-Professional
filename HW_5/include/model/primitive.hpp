#pragma once

#include <memory>
#if defined(_MSC_VER) && !defined(__clang__) && !defined(__INTEL_COMPILER)
#include <string>
#endif

namespace editor::model
{
    class Primitive
    {
    public:
        virtual ~Primitive() = default;

        [[nodiscard]] virtual auto getType() const -> std::string = 0;
    };
} // namespace editor::model
