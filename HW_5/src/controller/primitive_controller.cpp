#include <controller/primitive_controller.hpp>
#include <model/primitives/line.hpp>

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

    void PrimitiveController::removePrimitive(size_t const index)
    {
        if (document_ == nullptr)
        {
            return;
        }

        document_->removePrimitive(index);
    }
} // namespace editor::controller
