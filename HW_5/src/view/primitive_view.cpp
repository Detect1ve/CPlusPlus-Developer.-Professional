#include <iostream>

#include <model/primitives/circle.hpp>
#include <model/primitives/line.hpp>
#include <model/primitives/rectangle.hpp>
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

        [[nodiscard]] auto containsPoint(
            int const  /*x*/,
            int const  /*y*/) const -> bool override
        {
            const auto* line = dynamic_cast<const model::Line*>(getPrimitive());
            if (line == nullptr)
            {
                return false;
            }

            return false;
        }
    };

    class RectangleView : public PrimitiveView
    {
    public:
        explicit RectangleView(const model::Primitive* primitive)
            : PrimitiveView(primitive) {}

        void render() const override
        {
            const auto* rect = dynamic_cast<const model::Rectangle*>(getPrimitive());
            if (rect != nullptr)
            {
                std::cout << "Rendering Rectangle at (" << rect->getX() << ","
                    << rect->getY() << ") with size " << rect->getWidth() << "x"
                    << rect->getHeight() << " and thickness " << rect->getThickness()
                    << std::endl;
            }
        }

        [[nodiscard]] auto containsPoint(
            int const  /*x*/,
            int const  /*y*/) const -> bool override
        {
            const auto* rect = dynamic_cast<const model::Rectangle*>(getPrimitive());
            if (rect == nullptr)
            {
                return false;
            }

            return false;
        }
    };

    class CircleView : public PrimitiveView
    {
    public:
        explicit CircleView(const model::Primitive* primitive)
            : PrimitiveView(primitive) {}

        void render() const override
        {
            const auto* circle = dynamic_cast<const model::Circle*>(getPrimitive());
            if (circle != nullptr)
            {
                std::cout << "Rendering Circle at (" << circle->getCenterX() << ","
                    << circle->getCenterY() << ") with radius " << circle->getRadius()
                    << " and thickness " << circle->getThickness() << std::endl;
            }
        }

        [[nodiscard]] auto containsPoint(
            int const  /*x*/,
            int const  /*y*/) const -> bool override
        {
            const auto* circle = dynamic_cast<const model::Circle*>(getPrimitive());
            if (circle == nullptr)
            {
                return false;
            }

            return false;
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

        if (type == "Rectangle")
        {
            return std::make_unique<RectangleView>(primitive);
        }

        if (type == "Circle")
        {
            return std::make_unique<CircleView>(primitive);
        }

        return nullptr;
    }
} // namespace editor::view
