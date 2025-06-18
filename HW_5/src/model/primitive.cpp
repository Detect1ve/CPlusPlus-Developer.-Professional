#include <model/primitive.hpp>

namespace editor::model
{
    auto Primitive::deserialize(const std::string&  /*data*/)
        -> std::unique_ptr<Primitive>
    {
        return nullptr;
    }
} // namespace editor::model
