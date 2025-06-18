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

        void createRectangle(
            int x,
            int y,
            int width,
            int height,
            int thickness = 1);

        void createCircle(
            int centerX,
            int centerY,
            int radius,
            int thickness = 1);

        void removePrimitive(size_t index);

        void movePrimitive(
            size_t index,
            int    deltaX,
            int    deltaY);

        void resizePrimitive(
            size_t index,
            double scaleFactor);

    private:
        model::Document* document_;
    };

    using PrimitiveControllerPtr = std::unique_ptr<PrimitiveController>;
    using PrimitiveControllerWeakPtr = std::weak_ptr<PrimitiveController>;
    using PrimitiveControllerSharedPtr = std::shared_ptr<PrimitiveController>;
} // namespace editor::controller
