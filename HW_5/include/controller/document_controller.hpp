#ifndef CONTROLLER_DOCUMENT_CONTROLLER_HPP
#define CONTROLLER_DOCUMENT_CONTROLLER_HPP

#include <string_view> // std::string_view

#include <controller/primitive_controller.hpp>

namespace editor::controller
{
    class DocumentController
    {
    public:
        DocumentController();
        ~DocumentController() = default;
        DocumentController(const DocumentController&) = delete;
        DocumentController& operator=(const DocumentController&) = delete;
        DocumentController(DocumentController&&) = delete;
        DocumentController& operator=(DocumentController&&) = delete;

        ATTRIBUTE_PURE [[nodiscard]] model::Document* getDocument() const;
        ATTRIBUTE_PURE [[nodiscard]] PrimitiveController* getPrimitiveController() const;
        ATTRIBUTE_PURE [[nodiscard]] bool saveDocument(std::string_view filename) const;
        bool loadDocument(std::string_view filename);
        // NOLINTNEXTLINE(fuchsia-default-arguments-declarations)
        void createNewDocument(const std::string& name = "Untitled");

    private:
        std::unique_ptr<model::Document> document_;
        std::unique_ptr<PrimitiveController> primitiveController_;
    };
} // namespace editor::controller

#endif // CONTROLLER_DOCUMENT_CONTROLLER_HPP
