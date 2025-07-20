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

    bool DocumentController::saveDocument(const std::string& filename)
    {
        if (!document_) {
            return false;
        }

        return model::Document::saveToFile(*document_, filename);
    }

    bool DocumentController::loadDocument(const std::string& filename)
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

    model::Document* DocumentController::getDocument() const
    {
        return document_.get();
    }

    PrimitiveController* DocumentController::getPrimitiveController() const
    {
        return primitiveController_.get();
    }
} // namespace editor::controller
