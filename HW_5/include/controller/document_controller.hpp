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

        void createNewDocument(const std::string& name = "Untitled");
        auto saveDocument(const std::string& filename) -> bool;
        auto loadDocument(const std::string& filename) -> bool;

        [[nodiscard]] auto getDocument() const -> model::Document*;
        [[nodiscard]] auto getPrimitiveController() const -> PrimitiveController*;

    private:
        std::unique_ptr<model::Document> document_;
        std::unique_ptr<PrimitiveController> primitiveController_;
    };
} // namespace editor::controller
