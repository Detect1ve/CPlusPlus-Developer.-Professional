#ifndef VIEW_DOCUMENT_VIEW_HPP
#define VIEW_DOCUMENT_VIEW_HPP

#include <model/document.hpp>
#include <view/primitive_view.hpp>

namespace editor::view
{
    class DocumentView
    {
    public:
        explicit DocumentView(const model::Document* document);
        ~DocumentView() = default;

        DocumentView(const DocumentView&) = delete;
        DocumentView& operator=(const DocumentView&) = delete;
        DocumentView(DocumentView&&) = delete;
        DocumentView& operator=(DocumentView&&) = delete;

        void render() const;
        void update();

        [[nodiscard]] const model::Document* getDocument() const;

    private:
        const model::Document* document_;
        std::vector<std::unique_ptr<PrimitiveView>> primitiveViews_;

        void createPrimitiveViews();
    };
} // namespace editor::view

#endif /* VIEW_DOCUMENT_VIEW_HPP */
