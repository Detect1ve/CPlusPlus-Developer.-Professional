#include <model/document.hpp>

namespace editor::model
{
    Document::Document(const std::string& name) : name_(name) {}

    void Document::addPrimitive(std::unique_ptr<Primitive> primitive)
    {
        primitives_.emplace_back(std::move(primitive));
    }

    void Document::removePrimitive(size_t index)
    {
        if (index < primitives_.size())
        {
            primitives_.erase(primitives_.begin() + index);
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
    }

    auto Document::saveToFile(
        const Document&    /*document*/,
        const std::string& /*filename*/) -> bool
    {
        return true;
    }

    auto Document::loadFromFile(const std::string&  /*filename*/)
        -> std::unique_ptr<Document>
    {
        return std::make_unique<Document>("Loaded Document");
    }
} // namespace editor::model
