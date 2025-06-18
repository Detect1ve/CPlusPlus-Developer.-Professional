#pragma once

#include <model/primitive.hpp>

namespace editor::model
{
    class Rectangle : public Primitive
    {
    public:
        Rectangle(
            int x,
            int y,
            int width,
            int height,
            int thickness = 1);

        [[nodiscard]] auto getType() const -> std::string override
        {
            return "Rectangle";
        }

        [[nodiscard]] auto clone() const -> std::unique_ptr<Primitive> override;
        [[nodiscard]] auto serialize() const -> std::string override;

        [[nodiscard]] auto getX() const -> int
        {
            return x_;
        }

        [[nodiscard]] auto getY() const -> int
        {
            return y_;
        }

        [[nodiscard]] auto getWidth() const -> int
        {
            return width_;
        }

        [[nodiscard]] auto getHeight() const -> int
        {
            return height_;
        }

        [[nodiscard]] auto getThickness() const -> int
        {
            return thickness_;
        }

        void setX(int const x)
        {
            x_ = x;
        }

        void setY(int const y)
        {
            y_ = y;
        }

        void setWidth(int const width)
        {
            width_ = width;
        }

        void setHeight(int const height)
        {
            height_ = height;
        }

        void setThickness(int const thickness)
        {
            thickness_ = thickness;
        }

    private:
        int x_;
        int y_;
        int width_;
        int height_;
        int thickness_;
    };
} // namespace editor::model
