#include <memory> // std::unique_ptr
#include <string> // std::string

#include <custom_print.hpp>
#include <model/primitive.hpp> // editor::model::Primitive
#include <model/primitives/line.hpp>
#include <view/primitive_view.hpp>

namespace editor::view
{
    class LineView final : public PrimitiveView
    {
    public:
        ~LineView() override;
        LineView(const LineView&) = delete;
        LineView(LineView&&) = delete;
        LineView& operator=(const LineView&) = delete;
        LineView& operator=(LineView&&) = delete;

        explicit LineView(const model::Primitive* primitive) : PrimitiveView(primitive) {}

        void render() const override
        {
            const auto* line = dynamic_cast<const model::Line*>(getPrimitive());
            if (line != nullptr)
            {
                cp::println("Rendering Line from ({},{}) to ({},{}) with thickness {}",
                    line->getX1(), line->getY1(), line->getX2(), line->getY2(),
                    line->getThickness());
            }
        }
    };

    LineView::~LineView() = default;
    PrimitiveView::PrimitiveView(const model::Primitive* primitive)
        : primitive_(primitive) {}

    PrimitiveView::~PrimitiveView() = default;

    const model::Primitive* PrimitiveView::getPrimitive() const
    {
        return primitive_;
    }

    std::unique_ptr<PrimitiveView> PrimitiveViewFactory::createView(
        const model::Primitive* primitive)
    {
        if (primitive == nullptr)
        {
            return nullptr;
        }

        const std::string type = primitive->getType();

        if (type == "Line")
        {
            return std::make_unique<LineView>(primitive);
        }

        return nullptr;
    }
} // namespace editor::view
