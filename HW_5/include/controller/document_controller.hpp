#pragma once

#include <controller/primitive_controller.hpp>

namespace editor::controller
{
    class DocumentController
    {
    public:
        DocumentController();
        ~DocumentController() = default;

        DocumentController(const DocumentController&) = delete;
        auto operator=(const DocumentController&) -> DocumentController& = delete;

        DocumentController(DocumentController&&) = default;
        auto operator=(DocumentController&&) -> DocumentController& = default;

        void createNewDocument(const std::string& name = "Untitled");
        auto saveDocument(const std::string& filename) -> bool;
        auto loadDocument(const std::string& filename) -> bool;

        auto getDocument() -> model::Document*;
        [[nodiscard]] auto getDocument() const -> const model::Document*;
        auto getPrimitiveController() -> PrimitiveController*;
        [[nodiscard]] auto getPrimitiveController() const -> const PrimitiveController*;

    private:
        std::unique_ptr<model::Document> document_;
        std::unique_ptr<PrimitiveController> primitiveController_;
    };

    using DocumentControllerPtr = std::unique_ptr<DocumentController>;
    using DocumentControllerWeakPtr = std::weak_ptr<DocumentController>;
    using DocumentControllerSharedPtr = std::shared_ptr<DocumentController>;
} // namespace editor::controller
