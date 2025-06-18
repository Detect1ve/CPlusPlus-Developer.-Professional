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

        Document(Document&&) = default;
        auto operator=(Document&&) -> Document& = default;

        void addPrimitive(std::unique_ptr<Primitive> primitive);
        void removePrimitive(size_t index);
        [[nodiscard]] auto getPrimitive(size_t index) const -> const Primitive*;
        [[nodiscard]] auto getPrimitiveCount() const -> size_t;

        [[nodiscard]] auto getName() const -> const std::string&;
        void setName(const std::string& name);

        [[nodiscard]] auto serialize() const -> std::string;
        static auto deserialize(const std::string& data) -> std::unique_ptr<Document>;

        [[nodiscard]] auto saveToFile(const std::string& filename) const -> bool;
        static auto loadFromFile(const std::string& filename) ->
            std::unique_ptr<Document>;

    private:
        std::string name_;
        std::vector<std::unique_ptr<Primitive>> primitives_;
        bool modified_;
    };

    using DocumentPtr = std::unique_ptr<Document>;
    using DocumentWeakPtr = std::weak_ptr<Document>;
    using DocumentSharedPtr = std::shared_ptr<Document>;
} // namespace editor::model
