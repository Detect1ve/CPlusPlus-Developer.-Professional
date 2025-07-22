#ifndef CONTROLLER_PRIMITIVE_CONTROLLER_HPP
#define CONTROLLER_PRIMITIVE_CONTROLLER_HPP

#include <model/document.hpp>

namespace editor::controller
{
    class PrimitiveController
    {
    public:
        explicit PrimitiveController(model::Document* document);
        ~PrimitiveController() = default;

        PrimitiveController(const PrimitiveController&) = delete;
        PrimitiveController& operator=(const PrimitiveController&) = delete;
        PrimitiveController(PrimitiveController&&) = delete;
        PrimitiveController& operator=(PrimitiveController&&) = delete;

        void createLine(
            struct Point start,
            struct Point end,
            int          thickness);

        void removePrimitive(size_t index);

    private:
        model::Document* document_;
    };
} // namespace editor::controller

#endif /* CONTROLLER_PRIMITIVE_CONTROLLER_HPP */
