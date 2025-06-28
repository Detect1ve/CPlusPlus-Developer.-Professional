#pragma once

#include <Eigen/Dense>

class MLP
{
public:
    MLP(
        const std::string& w1_path,
        const std::string& w2_path);

    auto loadWeights(
        const std::string& w1_path,
        const std::string& w2_path) -> bool;

    auto evaluate(const std::string& test_data_path) -> float;

    static auto evaluate_with_predictions(
        const std::string& test_data_path,
        const std::string& predictions_path) -> float;

private:
    Eigen::MatrixXf W1;

    Eigen::MatrixXf W2;

    auto predict(const std::vector<float>& image) -> int;
    static auto sigma(const Eigen::VectorXf& z) -> Eigen::VectorXf;
    static auto softmax(const Eigen::VectorXf& z) -> Eigen::VectorXf;
};
