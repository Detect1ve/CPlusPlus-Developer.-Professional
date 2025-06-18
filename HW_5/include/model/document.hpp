#pragma once

#include <vector>

#include <model/primitive.hpp>

namespace editor::model
{
    class Document
    {
    public:
        Document(const std::string& name = "Untitled");
        ~Document() = default;

        Document(const Document&) = delete;
        auto operator=(const Document&) -> Document& = delete;

        void addPrimitive(std::unique_ptr<Primitive> primitive);
        void removePrimitive(size_t index);
        [[nodiscard]] auto getPrimitive(size_t index) const -> const Primitive*;
        [[nodiscard]] auto getPrimitiveCount() const -> size_t;

        [[nodiscard]] auto getName() const -> const std::string&;
        void setName(const std::string& name);

        [[nodiscard]] static auto saveToFile(
            const Document&    document,
            const std::string& filename) -> bool;

        static auto loadFromFile(const std::string& filename) ->
            std::unique_ptr<Document>;

    private:
        std::string name_;
        std::vector<std::unique_ptr<Primitive>> primitives_;
    };
} // namespace editor::model
