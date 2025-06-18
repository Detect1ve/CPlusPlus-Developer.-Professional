#include <model/primitives/circle.hpp>

namespace editor::model
{
    Circle::Circle(
        int const centerX,
        int const centerY,
        int const radius,
        int const thickness)
        :
        centerX_(centerX),
        centerY_(centerY),
        radius_(radius),
        thickness_(thickness) {}

    auto Circle::clone() const -> std::unique_ptr<Primitive>
    {
        return std::make_unique<Circle>(centerX_, centerY_, radius_, thickness_);
    }

    auto Circle::serialize() const -> std::string
    {
        return "Circle";
    }
} // namespace editor::model
