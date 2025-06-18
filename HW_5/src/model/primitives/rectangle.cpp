#include <model/primitives/rectangle.hpp>

namespace editor::model
{
    Rectangle::Rectangle(
        int const x,
        int const y,
        int const width,
        int const height,
        int const thickness)
        :
        x_(x),
        y_(y),
        width_(width),
        height_(height),
        thickness_(thickness) {}

    auto Rectangle::clone() const -> std::unique_ptr<Primitive>
    {
        return std::make_unique<Rectangle>(x_, y_, width_, height_, thickness_);
    }

    auto Rectangle::serialize() const -> std::string
    {
        return "Rectangle";
    }
} // namespace editor::model
