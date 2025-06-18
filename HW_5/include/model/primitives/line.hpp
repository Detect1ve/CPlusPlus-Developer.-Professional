#pragma once

#include <model/primitive.hpp>

namespace editor::model
{
    class Line : public Primitive
    {
    public:
        Line(
            int x1,
            int y1,
            int x2,
            int y2,
            int thickness = 1)
            :
            x1_(x1),
            y1_(y1),
            x2_(x2),
            y2_(y2),
            thickness_(thickness) {}

        [[nodiscard]] auto getType() const -> std::string override
        {
            return "Line";
        }

        [[nodiscard]] auto getX1() const -> int
        {
            return x1_;
        }

        [[nodiscard]] auto getY1() const -> int
        {
            return y1_;
        }

        [[nodiscard]] auto getX2() const -> int
        {
            return x2_;
        }

        [[nodiscard]] auto getY2() const -> int
        {
            return y2_;
        }

        [[nodiscard]] auto getThickness() const -> int
        {
            return thickness_;
        }

    private:
        int x1_;
        int y1_;
        int x2_;
        int y2_;
        int thickness_;
    };
} // namespace editor::model
