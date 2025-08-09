#include <gtest/gtest.h>

#include <mlp.hpp>

TEST(HW13, AccuracyWithPredictions)
{
    const float expected_accuracy = 0.9F;
    const std::string test_data_path = std::string(PROJECT_SOURCE_DIR) + "/test.csv";
    const std::string predictions_path =
        std::string(PROJECT_SOURCE_DIR) + "/test_data_mlp.txt";
    const float accuracy =
        MLP::evaluate_with_predictions(test_data_path, predictions_path);

    ASSERT_NEAR(accuracy, expected_accuracy, 0.001F);
}
