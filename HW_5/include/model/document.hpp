#ifndef MODEL_DOCUMENT_HPP
#define MODEL_DOCUMENT_HPP

#if __cplusplus <= 201703L
#include <string>
#endif
#include <vector>

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

        void addPrimitive(std::unique_ptr<Primitive> primitive);
        void removePrimitive(size_t index);
        [[nodiscard]] const Primitive* getPrimitive(size_t index) const;
        [[nodiscard]] size_t getPrimitiveCount() const;

        [[nodiscard]] const std::string& getName() const;
        void setName(const std::string& name);

        [[nodiscard]] static bool saveToFile(
            const Document&    document,
            const std::string& filename);

        static std::unique_ptr<Document> loadFromFile(const std::string& filename);

    private:
        std::string name_;
        std::vector<std::unique_ptr<Primitive>> primitives_;
    };
} // namespace editor::model

#endif /* MODEL_DOCUMENT_HPP */
