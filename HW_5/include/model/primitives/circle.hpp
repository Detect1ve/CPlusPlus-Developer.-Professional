#pragma once

#include <model/primitive.hpp>

namespace editor::model
{
    class Circle : public Primitive
    {
    public:
        Circle(int centerX, int centerY, int radius, int thickness = 1);

        [[nodiscard]] auto getType() const -> std::string override
        {
            return "Circle";
        }

        [[nodiscard]] auto clone() const -> std::unique_ptr<Primitive> override;
        [[nodiscard]] auto serialize() const -> std::string override;

        [[nodiscard]] auto getCenterX() const -> int
        {
            return centerX_;
        }

        [[nodiscard]] auto getCenterY() const -> int
        {
            return centerY_;
        }

        [[nodiscard]] auto getRadius() const -> int
        {
            return radius_;
        }

        [[nodiscard]] auto getThickness() const -> int
        {
            return thickness_;
        }

        void setCenterX(int const centerX)
        {
            centerX_ = centerX;
        }

        void setCenterY(int const centerY)
        {
            centerY_ = centerY;
        }

        void setRadius(int const radius)
        {
            radius_ = radius;
        }

        void setThickness(int const thickness)
        {
            thickness_ = thickness;
        }

    private:
        int centerX_;
        int centerY_;
        int radius_;
        int thickness_;
    };
} // namespace editor::model
