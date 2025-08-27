#ifndef CONTROLLER_DOCUMENT_CONTROLLER_HPP
#define CONTROLLER_DOCUMENT_CONTROLLER_HPP

#include <controller/primitive_controller.hpp>

namespace editor::controller
{
    class DocumentController
    {
        std::unique_ptr<model::Document> document_;
        std::unique_ptr<PrimitiveController> primitiveController_;
    public:
        DocumentController();
        ~DocumentController() = default;

        DocumentController(const DocumentController&) = delete;
        DocumentController& operator=(const DocumentController&) = delete;
        DocumentController(DocumentController&&) = delete;
        DocumentController& operator=(DocumentController&&) = delete;

        // NOLINTNEXTLINE(fuchsia-default-arguments-declarations)
        void createNewDocument(const std::string& name = "Untitled");
        bool saveDocument(const std::string& filename)
#if !defined(_MSC_VER)
            __attribute__((pure))
#endif
            ;
        bool loadDocument(const std::string& filename);

        [[nodiscard]] model::Document* getDocument() const
#if !defined(_MSC_VER)
            __attribute__((pure))
#endif
            ;
        [[nodiscard]] PrimitiveController* getPrimitiveController() const
#if !defined(_MSC_VER)
            __attribute__((pure))
#endif
            ;
    };
} // namespace editor::controller

#endif // CONTROLLER_DOCUMENT_CONTROLLER_HPP
