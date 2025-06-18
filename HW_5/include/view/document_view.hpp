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

        void render() const;
        void update();

        [[nodiscard]] auto getDocument() const -> const model::Document*;

    private:
        const model::Document* document_;
        std::vector<std::unique_ptr<PrimitiveView>> primitiveViews_;

        void createPrimitiveViews();
    };
} // namespace editor::view
