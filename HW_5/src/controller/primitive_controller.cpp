#include <controller/primitive_controller.hpp>
#include <model/primitives/line.hpp>
#include <model/primitives/rectangle.hpp>
#include <model/primitives/circle.hpp>

namespace editor::controller
{
    PrimitiveController::PrimitiveController(model::Document* document)
        : document_(document) {}

    void PrimitiveController::createLine(
        int const x1,
        int const y1,
        int const x2,
        int const y2,
        int const thickness)
    {
        if (document_ == nullptr)
        {
            return;
        }

        auto line = std::make_unique<model::Line>(x1, y1, x2, y2, thickness);
        document_->addPrimitive(std::move(line));
    }

    void PrimitiveController::createRectangle(
        int const x,
        int const y,
        int const width,
        int const height,
        int const thickness)
    {
        if (document_ == nullptr)
        {
            return;
        }

        auto rectangle = std::make_unique<model::Rectangle>(x, y, width, height,
            thickness);

        document_->addPrimitive(std::move(rectangle));
    }

    void PrimitiveController::createCircle(
        int const centerX,
        int const centerY,
        int const radius,
        int const thickness)
    {
        if (document_ == nullptr)
        {
            return;
        }

        auto circle = std::make_unique<model::Circle>(centerX, centerY, radius,
            thickness);

        document_->addPrimitive(std::move(circle));
    }

    void PrimitiveController::removePrimitive(size_t const index)
    {
        if (document_ == nullptr)
        {
            return;
        }

        document_->removePrimitive(index);
    }

    void PrimitiveController::movePrimitive(
        size_t const  /*index*/,
        int const     /*deltaX*/,
        int const     /*deltaY*/)
    {
        if (document_ == nullptr)
        {
            return;
        }
    }

    void PrimitiveController::resizePrimitive(
        size_t const  /*index*/,
        double const  /*scaleFactor*/)
    {
        if (document_ == nullptr)
        {
            return;
        }
    }
} // namespace editor::controller
