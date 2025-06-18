#pragma once

#include <model/primitive.hpp>

namespace editor::view
{
    class PrimitiveView {
    public:
        explicit PrimitiveView(const model::Primitive* primitive);
        virtual ~PrimitiveView() = default;

        virtual void render() const = 0;

        [[nodiscard]] auto getPrimitive() const -> const model::Primitive*;

    private:
        const model::Primitive* primitive_;
    };

    class PrimitiveViewFactory {
    public:
        static auto createView(const model::Primitive* primitive)
            -> std::unique_ptr<PrimitiveView>;
    };
} // namespace editor::view
