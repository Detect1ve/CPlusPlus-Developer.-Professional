#include <memory> // std::make_unique
#include <string> // std::string
#include <string_view> // std::string_view
#include <utility> // std::move

#include <controller/document_controller.hpp>
#include <controller/primitive_controller.hpp> // editor::controller::PrimitiveController
#include <model/document.hpp> // editor::model::Document

namespace editor::controller
{
    DocumentController::DocumentController()
    :
    document_(nullptr),
    primitiveController_(nullptr)
    {
        createNewDocument();
    }

    void DocumentController::createNewDocument(const std::string& name)
    {
        document_ = std::make_unique<model::Document>(name);
        primitiveController_ = std::make_unique<PrimitiveController>(document_.get());
    }

    bool DocumentController::saveDocument(const std::string_view filename)
    {
        if (!document_) {
            return false;
        }

        return model::Document::saveToFile(*document_, filename);
    }

    bool DocumentController::loadDocument(const std::string_view filename)
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
