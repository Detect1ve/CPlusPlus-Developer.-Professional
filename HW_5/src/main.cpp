#include <cstdio> // stderr
#include <cstdlib> // EXIT_FAILURE
#include <exception> // std::exception
#include <memory> // std::make_unique

#include <controller/document_controller.hpp>
#include <custom_print.hpp>
#include <model/primitive.hpp> // Point
#include <view/document_view.hpp>

int main()
{
    constexpr int THICKNESS = 1;
    Point const start {.x = 10, .y = 10};
    Point const end {.x = 100, .y = 100};

    try
    {
        auto docController = std::make_unique<editor::controller::DocumentController>();

        docController->createNewDocument("New Document");

        auto documentView =
            std::make_unique<editor::view::DocumentView>(docController->getDocument());

        docController->getPrimitiveController()->createLine(start, end, THICKNESS);

        docController->getPrimitiveController()->removePrimitive(0);

        if (!docController->saveDocument("document.vec"))
        {
            cp::println(stderr, "Cannot save the document");
        }

        docController->loadDocument("document.vec");

        documentView = std::make_unique<editor::view::DocumentView>(
            docController->getDocument());
        documentView->render();
    }
    catch (const std::exception& e)
    {
        cp::safe_error(e.what());

        return EXIT_FAILURE;
    }
    catch (...)
    {
        cp::safe_error();

        return EXIT_FAILURE;
    }

    return 0;
}
