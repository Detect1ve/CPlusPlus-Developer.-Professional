#include <model/document.hpp>

namespace editor::model
{
    Document::Document(std::string name) : name_(std::move(name)) {}

    void Document::addPrimitive(std::unique_ptr<Primitive> primitive)
    {
        primitives_.emplace_back(std::move(primitive));
    }

    void Document::removePrimitive(std::size_t index)
    {
        using diff_t = decltype(primitives_)::difference_type;

        if (index < primitives_.size())
        {
            primitives_.erase(primitives_.begin() + static_cast<diff_t>(index));
        }
    }

    const Primitive* Document::getPrimitive(std::size_t index) const
    {
        if (index < primitives_.size())
        {
            return primitives_[index].get();
        }

        return nullptr;
    }

    std::size_t Document::getPrimitiveCount() const
    {
        return primitives_.size();
    }

    const std::string& Document::getName() const
    {
        return name_;
    }

    void Document::setName(const std::string& name)
    {
        name_ = name;
    }

    bool Document::saveToFile(
        const Document&    /*document*/,
        const std::string& /*filename*/)
    {
        return true;
    }

    std::unique_ptr<Document> Document::loadFromFile(const std::string&  /*filename*/)
    {
        return std::make_unique<Document>("Loaded Document");
    }
} // namespace editor::model
