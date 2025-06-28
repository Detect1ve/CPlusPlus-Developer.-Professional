#include <iostream>

#include <mlp.h>

void printUsage(const char *const programName)
{
    std::cout << "Usage: " << programName << " <test_data_path> <model_dir>" << std::endl;
    std::cout << "Example: " << programName << " test.csv model" << std::endl;
}

auto main(
    const int   argc,
    const char* argv[]) -> int
{
    int ret = 0;

    if (argc != 3)
    {
        std::cerr << "Error: Invalid number of arguments!" << std::endl;
        printUsage(argv[0]);
        ret = -1;

        return ret;
    }

    std::string test_data_path = argv[1];
    std::string model_dir = argv[2];

    std::string w1_path = model_dir + "/w1.txt";
    std::string w2_path = model_dir + "/w2.txt";

    try
    {
        MLP model(w1_path, w2_path);

        float accuracy = model.evaluate(test_data_path);

        std::cout.precision(3);
        std::cout << std::fixed << accuracy << std::endl;

    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        ret = -2;
    }

    return 0;
}
