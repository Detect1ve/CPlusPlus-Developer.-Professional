#ifndef MODEL_DOCUMENT_HPP
#define MODEL_DOCUMENT_HPP

#include <vector>

#include <attribute_wrapper.hpp>

#include <model/primitive.hpp>

namespace editor::model
{
    class Document
    {
    public:
        explicit Document(std::string name) : name_(std::move(name)) {}
        ~Document() = default;
        Document(const Document&) = delete;
        Document& operator=(const Document&) = delete;
        Document(Document&&) = delete;
        Document& operator=(Document&&) = delete;

        void addPrimitive(std::unique_ptr<Primitive> primitive);
        void removePrimitive(std::size_t index);
        ATTRIBUTE_PURE [[nodiscard]] const Primitive* getPrimitive(std::size_t index)
            const;
        ATTRIBUTE_PURE [[nodiscard]] std::size_t getPrimitiveCount() const;
        ATTRIBUTE_CONST [[nodiscard]] std::string_view getName() const;
        void setName(std::string_view name);
        ATTRIBUTE_CONST [[nodiscard]] static bool saveToFile(
            const Document&  document,
            std::string_view filename);
        static std::unique_ptr<Document> loadFromFile(std::string_view filename);

    private:
        std::string name_;
        std::vector<std::unique_ptr<Primitive>> primitives_;
    };
} // namespace editor::model

#endif // MODEL_DOCUMENT_HPP
