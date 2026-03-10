#include <cstddef> // std::size_t
#include <memory> // std::make_unique
#include <utility> // std::move

#include <controller/primitive_controller.hpp>
#include <model/document.hpp> // editor::model::Document
#include <model/primitive.hpp> // Point
#include <model/primitives/line.hpp>

namespace editor::controller
{
    void PrimitiveController::createLine(
        Point const start,
        Point const end,
        int const   thickness)
    {
        if (document_ == nullptr)
        {
            return;
        }

        auto line = std::make_unique<model::Line>(start, end, thickness);
        document_->addPrimitive(std::move(line));
    }

    void PrimitiveController::removePrimitive(std::size_t const index)
    {
        if (document_ == nullptr)
        {
            return;
        }

        document_->removePrimitive(index);
    }
} // namespace editor::controller
