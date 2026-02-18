#ifndef CONTROLLER_DOCUMENT_CONTROLLER_HPP
#define CONTROLLER_DOCUMENT_CONTROLLER_HPP

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

        // NOLINTNEXTLINE(fuchsia-default-arguments-declarations)
        void createNewDocument(const std::string& name = "Untitled");
        ATTRIBUTE_PURE bool saveDocument(std::string_view filename);
        bool loadDocument(std::string_view filename);

        ATTRIBUTE_PURE [[nodiscard]] model::Document* getDocument() const;
        ATTRIBUTE_PURE [[nodiscard]] PrimitiveController* getPrimitiveController() const;

    private:
        std::unique_ptr<model::Document> document_;
        std::unique_ptr<PrimitiveController> primitiveController_;
    };
} // namespace editor::controller

#endif // CONTROLLER_DOCUMENT_CONTROLLER_HPP
