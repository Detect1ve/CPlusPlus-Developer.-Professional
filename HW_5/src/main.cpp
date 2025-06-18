#include <controller/document_controller.hpp>
#include <view/document_view.hpp>

auto main() -> int
{
    auto documentController = std::make_unique<editor::controller::DocumentController>();

    documentController->createNewDocument("New Document");

    auto documentView =
        std::make_unique<editor::view::DocumentView>(documentController->getDocument());

    documentController->getPrimitiveController()->createLine(10, 10, 100, 100);
    documentController->getPrimitiveController()->createRectangle(10, 10, 100, 50);
    documentController->getPrimitiveController()->createCircle(100, 100, 50);

    documentController->getPrimitiveController()->removePrimitive(0);

    documentController->saveDocument("document.vec");
    documentController->loadDocument("document.vec");

    documentView->render();

    return 0;
}
