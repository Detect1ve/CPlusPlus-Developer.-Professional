#pragma once

#include <model/document.hpp>

namespace editor::controller
{
    class PrimitiveController
    {
    public:
        explicit PrimitiveController(model::Document* document);
        ~PrimitiveController() = default;

        void createLine(
            int x1,
            int y1,
            int x2,
            int y2,
            int thickness = 1);

        void removePrimitive(size_t index);

    private:
        model::Document* document_;
    };
} // namespace editor::controller
