#pragma once

#include <memory>

namespace editor::model
{
    class Primitive
    {
    public:
        virtual ~Primitive() = default;

        [[nodiscard]] virtual auto getType() const -> std::string = 0;
    };
} // namespace editor::model
