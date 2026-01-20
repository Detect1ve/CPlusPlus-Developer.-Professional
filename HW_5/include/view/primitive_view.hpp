#ifndef VIEW_PRIMITIVE_VIEW_HPP
#define VIEW_PRIMITIVE_VIEW_HPP

#include <attribute_wrapper.hpp>

#include <model/primitive.hpp>

namespace editor::view
{
    class PrimitiveView
    {
        const model::Primitive* primitive_;
    public:
        explicit PrimitiveView(const model::Primitive* primitive);
        virtual ~PrimitiveView();

        PrimitiveView(const PrimitiveView&) = delete;
        PrimitiveView& operator=(const PrimitiveView&) = delete;
        PrimitiveView(PrimitiveView&&) = delete;
        PrimitiveView& operator=(PrimitiveView&&) = delete;

        virtual void render() const = 0;

        ATTRIBUTE_PURE [[nodiscard]] const model::Primitive* getPrimitive() const;
    };

    class PrimitiveViewFactory
    {
    public:
        static std::unique_ptr<PrimitiveView> createView(
            const model::Primitive* primitive);
    };
} // namespace editor::view

#endif // VIEW_PRIMITIVE_VIEW_HPP
