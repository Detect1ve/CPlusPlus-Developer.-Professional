#include <iostream>

#include <model/primitives/line.hpp>
#include <view/primitive_view.hpp>

namespace editor::view
{
    class LineView : public PrimitiveView
    {
    public:
        explicit LineView(const model::Primitive* primitive) : PrimitiveView(primitive) {}

        void render() const override
        {
            const auto* line = dynamic_cast<const model::Line*>(getPrimitive());
            if (line != nullptr)
            {
                std::cout << "Rendering Line from (" << line->getX1() << ","
                    << line->getY1() << ") to (" << line->getX2() << "," << line->getY2()
                    << ") with thickness " << line->getThickness() << std::endl;
            }
        }
    };

    PrimitiveView::PrimitiveView(const model::Primitive* primitive)
        : primitive_(primitive) {}

    auto PrimitiveView::getPrimitive() const -> const model::Primitive*
    {
        return primitive_;
    }

    auto PrimitiveViewFactory::createView(const model::Primitive* primitive)
        -> std::unique_ptr<PrimitiveView>
    {
        if (primitive == nullptr)
        {
            return nullptr;
        }

        std::string type = primitive->getType();

        if (type == "Line")
        {
            return std::make_unique<LineView>(primitive);
        }

        return nullptr;
    }
} // namespace editor::view
