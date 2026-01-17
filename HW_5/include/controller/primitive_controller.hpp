#ifndef CONTROLLER_PRIMITIVE_CONTROLLER_HPP
#define CONTROLLER_PRIMITIVE_CONTROLLER_HPP

#include <model/document.hpp>

namespace editor::controller
{
    class PrimitiveController
    {
        model::Document* document_;
    public:
        explicit PrimitiveController(model::Document* document);
        ~PrimitiveController() = default;

        PrimitiveController(const PrimitiveController&) = delete;
        PrimitiveController& operator=(const PrimitiveController&) = delete;
        PrimitiveController(PrimitiveController&&) = delete;
        PrimitiveController& operator=(PrimitiveController&&) = delete;

        void createLine(
            Point start,
            Point end,
            int   thickness);

        void removePrimitive(std::size_t index);
    };
} // namespace editor::controller

#endif // CONTROLLER_PRIMITIVE_CONTROLLER_HPP
