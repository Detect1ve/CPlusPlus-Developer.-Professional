#include <model/document.hpp>

namespace editor::model
{
    Document::Document(const std::string& name)
        :
        name_(name),
        modified_(false) {}

    void Document::addPrimitive(std::unique_ptr<Primitive> primitive)
    {
        primitives_.emplace_back(std::move(primitive));
        modified_ = true;
    }

    void Document::removePrimitive(size_t index)
    {
        if (index < primitives_.size())
        {
            primitives_.erase(primitives_.begin() + index);
            modified_ = true;
        }
    }

    auto Document::getPrimitive(size_t index) const -> const Primitive*
    {
        if (index < primitives_.size())
        {
            return primitives_[index].get();
        }

        return nullptr;
    }

    auto Document::getPrimitiveCount() const -> size_t
    {
        return primitives_.size();
    }

    auto Document::getName() const -> const std::string&
    {
        return name_;
    }

    void Document::setName(const std::string& name)
    {
        name_ = name;
        modified_ = true;
    }

    auto Document::serialize() const -> std::string
    {
        return "Document:" + name_;
    }

    auto Document::deserialize(const std::string&  /*data*/) -> std::unique_ptr<Document>
    {
        return std::make_unique<Document>("Deserialized Document");
    }

    auto Document::saveToFile(const std::string&  /*filename*/) const -> bool
    {
        return true;
    }

    auto Document::loadFromFile(const std::string&  /*filename*/)
        -> std::unique_ptr<Document>
    {
        return std::make_unique<Document>("Loaded Document");
    }
} // namespace editor::model
