# 5. Editor

In this task, try designing by creating a skeleton of a graphic editor.

Design the simplest graphic vector editor. Prepare class layouts that reflect the structure of the future project.

**The functionality for layout is as follows:**
- creating a new document
- importing a document from a file
- exporting a document to a file
- creating a graphic primitive
- deleting a graphic primitive

The primary focus should be on the Model-View-Controller (MVC) pattern and polymorphism. Functions that are GUI handlers should be collected in one file with the `main` function.

Attention should be focused on declarations, implementation only as a last resort for a minimal demonstration of the necessary calls. The project should compile, and all headers should pass the compilation stage.

**The task is considered successful when** all files have passed the compilation stage, diagrams cover all classes, and the code has successfully passed the analysis.

- **Additional recommendations**
1. Incorporate smart pointers into interfaces, thereby solving the issue of object ownership.
2. Remember the principle of single responsibility, divide the code into logical modules (classes, functions).
3. Avoid code duplication.
