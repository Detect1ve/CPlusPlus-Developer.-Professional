#include <model/primitives/line.hpp>

namespace editor::model
{
    Line::Line(
        int const x1,
        int const y1,
        int const x2,
        int const y2,
        int const thickness)
        :
        x1_(x1),
        y1_(y1),
        x2_(x2),
        y2_(y2),
        thickness_(thickness) {}

    auto Line::clone() const -> std::unique_ptr<Primitive>
    {
        return std::make_unique<Line>(x1_, y1_, x2_, y2_, thickness_);
    }

    auto Line::serialize() const -> std::string
    {
        return "Line";
    }
} // namespace editor::model
