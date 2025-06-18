#pragma once

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
        auto operator=(const DocumentView&) -> DocumentView& = delete;

        DocumentView(DocumentView&&) = default;
        auto operator=(DocumentView&&) -> DocumentView& = default;

        void render() const;

        void update();

        [[nodiscard]] auto getPrimitiveViewAt(
            int x,
            int y) const -> const PrimitiveView*;

        [[nodiscard]] auto getDocument() const -> const model::Document*;

    private:
        const model::Document* document_;
        std::vector<std::unique_ptr<PrimitiveView>> primitiveViews_;

        void createPrimitiveViews();
    };

    using DocumentViewPtr = std::unique_ptr<DocumentView>;
    using DocumentViewWeakPtr = std::weak_ptr<DocumentView>;
    using DocumentViewSharedPtr = std::shared_ptr<DocumentView>;
} // namespace editor::view
