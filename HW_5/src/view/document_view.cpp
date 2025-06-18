#include <iostream>

#include <view/document_view.hpp>

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
            std::cout << "No document to render" << std::endl;
            return;
        }

        std::cout << "Rendering document: " << document_->getName() << std::endl;
        std::cout << "Number of primitives: " << primitiveViews_.size() << std::endl;

        for (const auto& view : primitiveViews_)
        {
            view->render();
        }
    }

    void DocumentView::update()
    {
        createPrimitiveViews();
    }

    auto DocumentView::getDocument() const -> const model::Document*
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

        for (size_t i = 0; i < document_->getPrimitiveCount(); ++i)
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
