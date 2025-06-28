#if __GNUC__ < 14\
|| __cplusplus <=  202002L
#include <charconv>
#endif
#include <fstream>
#include <iostream>

#include <Eigen/Dense>

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

class MLP::Impl
{
public:
    Eigen::MatrixXf W1;
    Eigen::MatrixXf W2;
};

MLP::MLP(
    const std::string& w1_path,
    const std::string& w2_path)
    :
    pimpl(std::make_unique<Impl>())
{
    if (!loadWeights(w1_path, w2_path))
    {
        std::cerr << "Failed to load weights!\n";
    }
}

MLP::~MLP() = default;

MLP::MLP(MLP&&) noexcept = default;
MLP& MLP::operator=(MLP&&) noexcept = default;

bool MLP::loadWeights(
    const std::string& w1_path,
    const std::string& w2_path)
{
    int row = 0;
    std::ifstream w1_file(w1_path);
    std::ifstream w2_file(w2_path);
    std::string line;

    if (!w1_file.is_open())
    {
        std::cerr << "Could not open file: " << w1_path << '\n';

        return false;
    }

    pimpl->W1.resize(784, 128);

    while (std::getline(w1_file, line) && row < 784)
    {
        float value = NAN;
        int col = 0;
        std::istringstream iss(line);

        while (iss >> value && col < 128)
        {
            pimpl->W1(row, col) = value;
            col++;
        }

        row++;
    }

    w1_file.close();

    if (!w2_file.is_open())
    {
        std::cerr << "Could not open file: " << w2_path << '\n';

        return false;
    }

    pimpl->W2.resize(128, 10);

    row = 0;
    while (std::getline(w2_file, line) && row < 128)
    {
        float value = NAN;
        int col = 0;
        std::istringstream iss(line);

        while (iss >> value && col < 10)
        {
            pimpl->W2(row, col) = value;
            col++;
        }

        row++;
    }

    w2_file.close();

    return true;
}

// NOLINTBEGIN(readability-identifier-length)
int MLP::predict(const std::vector<float>& image)
{
    Eigen::VectorXf x = Eigen::Map<const Eigen::VectorXf>(image.data(),
        static_cast<Eigen::Index>(image.size()));

    x = x / MLP::PIXEL_MAX_VALUE;

    // z_1 = W_1^T * x
    const Eigen::VectorXf z1 = pimpl->W1.transpose() * x;

    // o_1 = sigma(z_1)
    const Eigen::VectorXf o1 = sigma(z1);

    // z_2 = W_2^T * o_1
    const Eigen::VectorXf z2 = pimpl->W2.transpose() * o1;

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
    int correct = 0;
    int total = 0;
    std::ifstream pred_file(predictions_path);
    std::ifstream test_file(test_data_path);
    std::string pred_line;
    std::string test_line;

    if (!test_file.is_open())
    {
        std::cerr << "Could not open test file: " << test_data_path << '\n';
        return 0.0F;
    }

    if (!pred_file.is_open())
    {
        std::cerr << "Could not open predictions file: " << predictions_path << '\n';

        return 0.0F;
    }

    while (std::getline(test_file, test_line) && std::getline(pred_file, pred_line))
    {
        int true_label = 0;
        int predicted_label = 0;
        std::istringstream pred_iss(pred_line);
        std::istringstream test_iss(test_line);
        std::string token;

        if (!std::getline(test_iss, token, ','))
        {
            std::cerr << "Error reading class label from test file\n";
            continue;
        }

        const std::string_view token_sv(token);
        auto [ptr, ec] = std::from_chars(token_sv.data(),
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            token_sv.data() + token_sv.size(), true_label);
        if (ec != std::errc{})
        {
            std::cerr << "Error converting class label from test file: " << token << '\n';
            continue;
        }

        pred_iss >> predicted_label;

        if (predicted_label == true_label)
        {
            correct++;
        }

        total++;
    }

    test_file.close();
    pred_file.close();

    return static_cast<float>(correct) / static_cast<float>(total);
}

float MLP::evaluate(const std::string& test_data_path)
{
    int correct = 0;
    int total = 0;
    std::ifstream test_file(test_data_path);
    std::string line;

    if (!test_file.is_open())
    {
        std::cerr << "Could not open file: " << test_data_path << '\n';

        return 0.0F;
    }


    while (std::getline(test_file, line))
    {
        int true_label = 0;
        std::istringstream iss(line);
        std::string token;
        std::vector<float> image(784);

        if (!std::getline(iss, token, ','))
        {
            std::cerr << "Error reading class label\n";
            continue;
        }

        {
            const std::string_view token_sv(token);
            auto [ptr, ec] = std::from_chars(token_sv.data(),
                // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                token_sv.data() + token_sv.size(), true_label);
            if (ec != std::errc{})
            {
                std::cerr << "Error converting class label: " << token << '\n';
                continue;
            }
        }

        for (int i = 0; i < 784; ++i)
        {
            float pixel_value = 0.0F;

            if (!std::getline(iss, token, ','))
            {
                std::cerr << "Error reading pixel at position " << i << '\n';
                break;
            }

            const std::string_view token_sv(token);
            auto [ptr, ec] = std::from_chars(token_sv.data(),
                // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                token_sv.data() + token_sv.size(), pixel_value);
            if (ec != std::errc{})
            {
                std::cerr << "Error converting pixel at position " << i << ": "
                    << token_sv << '\n';
                pixel_value = 0.0F;
            }

            image[i] = pixel_value;
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
