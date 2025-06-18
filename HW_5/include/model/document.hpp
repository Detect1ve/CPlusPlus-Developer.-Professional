#ifndef MODEL_DOCUMENT_HPP
#define MODEL_DOCUMENT_HPP

#include <string_view> // std::string_view
#include <vector>

#include <attribute_wrapper.hpp>

#include <model/primitive.hpp>

namespace editor::model
{
    class Document
    {
    public:
        explicit Document(std::string name);
        ~Document() = default;
        Document(const Document&) = delete;
        Document& operator=(const Document&) = delete;
        Document(Document&&) = delete;
        Document& operator=(Document&&) = delete;

        ATTRIBUTE_CONST [[nodiscard]] std::string_view getName() const;
        ATTRIBUTE_CONST [[nodiscard]] static bool saveToFile(
            const Document&  document,
            std::string_view filename);
        ATTRIBUTE_PURE [[nodiscard]] const Primitive* getPrimitive(std::size_t index)
            const;
        ATTRIBUTE_PURE [[nodiscard]] std::size_t getPrimitiveCount() const;
        static std::unique_ptr<Document> loadFromFile(std::string_view filename);
        void addPrimitive(std::unique_ptr<Primitive> primitive);
        void removePrimitive(std::size_t index);
        void setName(std::string_view name);

    private:
        std::string name_;
        std::vector<std::unique_ptr<Primitive>> primitives_;
    };
} // namespace editor::model

#endif // MODEL_DOCUMENT_HPP
