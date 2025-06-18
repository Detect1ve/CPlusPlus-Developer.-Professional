#include <controller/document_controller.hpp>

namespace editor::controller
{
    DocumentController::DocumentController()
    {
        createNewDocument();
    }

    void DocumentController::createNewDocument(const std::string& name)
    {
        document_ = std::make_unique<model::Document>(name);
        primitiveController_ = std::make_unique<PrimitiveController>(document_.get());
    }

    auto DocumentController::saveDocument(const std::string& filename) -> bool
    {
        if (!document_) {
            return false;
        }

        return model::Document::saveToFile(*document_, filename);
    }

    auto DocumentController::loadDocument(const std::string& filename) -> bool
    {
        auto newDocument = model::Document::loadFromFile(filename);

        if (!newDocument)
        {
            return false;
        }

        document_ = std::move(newDocument);
        primitiveController_ = std::make_unique<PrimitiveController>(document_.get());

        return true;
    }

    auto DocumentController::getDocument() const -> model::Document*
    {
        return document_.get();
    }

    auto DocumentController::getPrimitiveController() const -> PrimitiveController*
    {
        return primitiveController_.get();
    }
} // namespace editor::controller
