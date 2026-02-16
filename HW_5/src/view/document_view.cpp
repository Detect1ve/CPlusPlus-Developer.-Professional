#include <cstddef> // std::size_t
#include <utility> // std::move

#include <custom_print.hpp>
#include <model/document.hpp> // editor::model::Document
#include <model/primitive.hpp> // editor::model::Primitive
#include <view/document_view.hpp>
#include <view/primitive_view.hpp> // editor::view::PrimitiveViewFactory

namespace editor::view
{
    DocumentView::DocumentView(const model::Document* document) : document_(document)
    {
        createPrimitiveViews();
    }

    void DocumentView::render() const
    {
        if (document_ == nullptr)
        {
            cp::println("No document to render");

            return;
        }

        cp::println("Rendering document: {}", document_->getName());
        cp::println("Number of primitives: {}", primitiveViews_.size());

        for (const auto& view : primitiveViews_)
        {
            view->render();
        }
    }

    void DocumentView::update()
    {
        createPrimitiveViews();
    }

    const model::Document* DocumentView::getDocument() const
    {
        return document_;
    }

    void DocumentView::createPrimitiveViews()
    {
        primitiveViews_.clear();

        if (document_ == nullptr)
        {
            return;
        }

        for (std::size_t i = 0; i < document_->getPrimitiveCount(); ++i)
        {
            const model::Primitive* primitive = document_->getPrimitive(i);
            auto view = PrimitiveViewFactory::createView(primitive);

            if (view)
            {
                primitiveViews_.emplace_back(std::move(view));
            }
        }
    }
} // namespace editor::view
