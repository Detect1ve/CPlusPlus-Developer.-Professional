#ifndef MODEL_PRIMITIVES_LINE_HPP
#define MODEL_PRIMITIVES_LINE_HPP

#include <model/primitive.hpp>

namespace editor::model
{
    class Line final : public Primitive
    {
    public:
        Line(
            Point start,
            Point end,
            int   thickness)
            :
            x1_(start.x),
            y1_(start.y),
            x2_(end.x),
            y2_(end.y),
            thickness_(thickness) {}

        [[nodiscard]] std::string getType() const override
        {
            return "Line";
        }

        [[nodiscard]] int getX1() const
        {
            return x1_;
        }

        [[nodiscard]] int getY1() const
        {
            return y1_;
        }

        [[nodiscard]] int getX2() const
        {
            return x2_;
        }

        [[nodiscard]] int getY2() const
        {
            return y2_;
        }

        [[nodiscard]] int getThickness() const
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

#endif // MODEL_PRIMITIVES_LINE_HPP
