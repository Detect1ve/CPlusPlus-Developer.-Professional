#if __GNUC__ < 14\
 || __cplusplus <=  202002L
#include <charconv>
#endif
#include <cstdio> // stderr
#include <fstream>
#include <limits> // std::numeric_limits
#include <memory> // std::make_unique
#include <ranges>
#include <sstream> // std::istringstream
#include <string> // std::string
#include <string_view> // std::string_view
#include <system_error> // std::errc
#include <vector> // std::vector

#include <Eigen/Core>

#include <custom_print.hpp>
#include <mlp.hpp>

namespace
{
    // NOLINTNEXTLINE(readability-identifier-length)
    Eigen::VectorXf sigma(const Eigen::VectorXf& z)
    {
        return 1.0F / (1.0F + (-z).array().exp());
    }

    // NOLINTNEXTLINE(readability-identifier-length)
    Eigen::VectorXf softmax(const Eigen::VectorXf& z)
    {
        Eigen::VectorXf exp_z = z.array().exp();

        return exp_z.array() / exp_z.sum();
    }
} // namespace

struct MLP::Impl
{
    Eigen::MatrixXf W1;
    Eigen::MatrixXf W2;
};

MLP::MLP(
    const std::string& w1_path,
    const std::string& w2_path)
    :
    pimpl_(std::make_unique<Impl>())
{
    if (!loadWeights(w1_path, w2_path))
    {
        cp::println(stderr, "Failed to load weights!");
    }
}

MLP::~MLP() = default;

MLP::MLP(MLP&&) noexcept = default;
MLP& MLP::operator=(MLP&&) noexcept = default;

bool MLP::loadWeights(
    const std::string& w1_path,
    const std::string& w2_path)
{
    int row{};
    std::ifstream w1_file{w1_path};
    std::ifstream w2_file{w2_path};
    std::string line;

    if (!w1_file.is_open())
    {
        cp::println(stderr, "Could not open file: {}", w1_path);

        return false;
    }

    pimpl_->W1.resize(784, 128);

    while (std::getline(w1_file, line) && row < 784)
    {
        float value = std::numeric_limits<float>::quiet_NaN();
        int col{};
        std::istringstream iss(line);

        while (iss >> value && col < 128)
        {
            pimpl_->W1(row, col) = value;
            col++;
        }

        row++;
    }

    if (!w2_file.is_open())
    {
        cp::println(stderr, "Could not open file: {}", w2_path);

        return false;
    }

    pimpl_->W2.resize(128, 10);

    row = 0;
    while (std::getline(w2_file, line) && row < 128)
    {
        float value = std::numeric_limits<float>::quiet_NaN();
        int col{};
        std::istringstream iss(line);

        while (iss >> value && col < 10)
        {
            pimpl_->W2(row, col) = value;
            col++;
        }

        row++;
    }

    return true;
}

// NOLINTBEGIN(readability-identifier-length)
int MLP::predict(const std::vector<float>& image)
{
    Eigen::VectorXf x = Eigen::Map<const Eigen::VectorXf>(image.data(),
        static_cast<Eigen::Index>(image.size()));

    x = x / MLP::PIXEL_MAX_VALUE;

    // z_1 = W_1^T * x
    const Eigen::VectorXf z1 = pimpl_->W1.transpose() * x;

    // o_1 = sigma(z_1)
    const Eigen::VectorXf o1 = sigma(z1);

    // z_2 = W_2^T * o_1
    const Eigen::VectorXf z2 = pimpl_->W2.transpose() * o1;

    // o_2 = softmax(z_2)
    const Eigen::VectorXf o2 = softmax(z2);

    Eigen::VectorXf::Index maxIndex = 0;
    o2.maxCoeff(&maxIndex);

    return static_cast<int>(maxIndex);
}
// NOLINTEND(readability-identifier-length)

float MLP::evaluate_with_predictions(
    const std::string& test_data_path,
    const std::string& predictions_path)
{
    int correct{};
    int total{};
    std::ifstream pred_file{predictions_path};
    std::ifstream test_file{test_data_path};
    std::string pred_line;
    std::string test_line;

    if (!test_file.is_open())
    {
        cp::println(stderr, "Could not open test file: {}", test_data_path);

        return 0.0F;
    }

    if (!pred_file.is_open())
    {
        cp::println(stderr, "Could not open predictions file: {}", predictions_path);

        return 0.0F;
    }

    while (std::getline(test_file, test_line) && std::getline(pred_file, pred_line))
    {
        int true_label{};
        int predicted_label{};
        std::istringstream pred_iss(pred_line);
        std::istringstream test_iss(test_line);
        std::string token;

        if (!std::getline(test_iss, token, ','))
        {
            cp::println(stderr, "Error reading class label from test file");
            continue;
        }

        const std::string_view token_sv(token);
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        if (std::from_chars(token_sv.data(), token_sv.data() + token_sv.size(),
            true_label).ec != std::errc{})
        {
            cp::println(stderr, "Error converting class label from test file: {}", token);
            continue;
        }

        pred_iss >> predicted_label;

        if (predicted_label == true_label)
        {
            correct++;
        }

        total++;
    }

    return static_cast<float>(correct) / static_cast<float>(total);
}

float MLP::evaluate(const std::string& test_data_path)
{
    int correct{};
    int total{};
    std::ifstream test_file(test_data_path);
    std::string line;

    if (!test_file.is_open())
    {
        cp::println(stderr, "Could not open file: {}", test_data_path);

        return 0.0F;
    }


    while (std::getline(test_file, line))
    {
        int true_label{};
        std::istringstream iss(line);
        std::string token;
        std::vector<float> image(784);

        if (!std::getline(iss, token, ','))
        {
            cp::println(stderr, "Error reading class label");
            continue;
        }

        {
            const std::string_view token_sv(token);
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            if (std::from_chars(token_sv.data(), token_sv.data() + token_sv.size(),
                true_label).ec != std::errc{})
            {
                cp::println(stderr, "Error converting class label: {}", token);
                continue;
            }
        }

        for (auto [idx, pixel] : std::views::enumerate(image))
        {
            if (!std::getline(iss, token, ','))
            {
                cp::println(stderr, "Error reading pixel at position {}", idx);
                break;
            }

            const std::string_view token_sv(token);
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            if (std::from_chars(token_sv.data(), token_sv.data() + token_sv.size(),
                pixel).ec != std::errc{})
            {
                cp::println(stderr, "Error converting pixel at position {}: {}", idx,
                    token_sv);
                pixel = 0.0F;
            }
        }

        const int predicted_label = predict(image);
        if (predicted_label == true_label)
        {
            correct++;
        }

        total++;
    }

    test_file.close();

    return static_cast<float>(correct) / static_cast<float>(total);
}
