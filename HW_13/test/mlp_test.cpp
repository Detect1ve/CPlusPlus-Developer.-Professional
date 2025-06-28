#include <gtest/gtest.h>

#include <mlp.h>

TEST(HW13, accuracy_with_predictions)
{
    std::string test_data_path = std::string(PROJECT_SOURCE_DIR) + "/test.csv";
    std::string predictions_path = std::string(PROJECT_SOURCE_DIR) + "/test_data_mlp.txt";

    float expected_accuracy = 0.9F;

    float accuracy = MLP::evaluate_with_predictions(test_data_path, predictions_path);

    ASSERT_NEAR(accuracy, expected_accuracy, 0.001F);
}
