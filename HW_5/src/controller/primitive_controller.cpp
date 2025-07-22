#include <controller/primitive_controller.hpp>
#include <model/primitives/line.hpp>

namespace editor::controller
{
    PrimitiveController::PrimitiveController(model::Document* document)
        : document_(document) {}

    void PrimitiveController::createLine(
        struct Point const start,
        struct Point const end,
        int const          thickness)
    {
        if (document_ == nullptr)
        {
            return;
        }

        auto line = std::make_unique<model::Line>(start, end, thickness);
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
