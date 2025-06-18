#include <controller/document_controller.hpp>
#include <view/document_view.hpp>

int main()
{
    constexpr int kThickness = 1;
    struct Point const start {.x = 10, .y = 10};
    struct Point const end {.x = 100, .y = 100};

    auto documentController = std::make_unique<editor::controller::DocumentController>();

    documentController->createNewDocument("New Document");

    auto documentView =
        std::make_unique<editor::view::DocumentView>(documentController->getDocument());

    documentController->getPrimitiveController()->createLine(start, end, kThickness);

    documentController->getPrimitiveController()->removePrimitive(0);

    documentController->saveDocument("document.vec");
    documentController->loadDocument("document.vec");

    documentView = std::make_unique<editor::view::DocumentView>(
        documentController->getDocument());
    documentView->render();

    return 0;
}
