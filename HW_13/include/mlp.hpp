#ifndef MLP_HPP
#define MLP_HPP

#include <memory>
#include <vector>

class MLP
{
    class Impl;
    std::unique_ptr<Impl> pimpl;

    int predict(const std::vector<float>& image);
public:
    static constexpr float PIXEL_MAX_VALUE = 255.0F;

    MLP(
        const std::string& w1_path,
        const std::string& w2_path);
    ~MLP();

    MLP(const MLP&) = delete;
    MLP& operator=(const MLP&) = delete;
    MLP(MLP&&) noexcept;
    MLP& operator=(MLP&&) noexcept;


    bool loadWeights(
        const std::string& w1_path,
        const std::string& w2_path);

    float evaluate(const std::string& test_data_path);

    static float evaluate_with_predictions(
        const std::string& test_data_path,
        const std::string& predictions_path);
};

#endif // MLP_HPP
