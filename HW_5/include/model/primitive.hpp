#pragma once

#include <memory>

namespace editor::model
{
    class Primitive
    {
    public:
        virtual ~Primitive() = default;

        [[nodiscard]] virtual auto getType() const -> std::string = 0;

        [[nodiscard]] virtual auto clone() const -> std::unique_ptr<Primitive> = 0;

        [[nodiscard]] virtual auto serialize() const -> std::string = 0;

        static auto deserialize(const std::string& data) -> std::unique_ptr<Primitive>;
    };

    using PrimitivePtr = std::unique_ptr<Primitive>;
    using PrimitiveWeakPtr = std::weak_ptr<Primitive>;
    using PrimitiveSharedPtr = std::shared_ptr<Primitive>;
} // namespace editor::model
