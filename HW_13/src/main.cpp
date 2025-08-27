#include <iostream>
#if defined(__clang__)\
 || defined(_MSC_VER) && !defined(__clang__) && !defined(__INTEL_COMPILER)\
 || __GNUC__ < 14
#include <span>
#endif

#include <mlp.hpp>

namespace
{
    void printUsage(const char *const programName)
    {
        std::cout << "Usage: " << programName << " <test_data_path> <model_dir>\n";
        std::cout << "Example: " << programName << " test.csv model\n";
    }
} // namespace

int main(
    const int   argc,
    const char* argv[])
{
    const std::span<const char*> args(argv, static_cast<std::size_t>(argc));
    int ret = 0;

    if (args.size() != 3)
    {
        std::cerr << "Error: Invalid number of arguments!\n";
        printUsage(args[0]);
        ret = -1;

        return ret;
    }

    const std::string test_data_path(args[1]);
    const std::string model_dir(args[2]);

    const std::string w1_path = model_dir + "/w1.txt";
    const std::string w2_path = model_dir + "/w2.txt";

    try
    {
        MLP model(w1_path, w2_path);

        const float accuracy = model.evaluate(test_data_path);

        std::cout.precision(3);
        std::cout << std::fixed << accuracy << '\n';

    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        ret = -2;
    }

    return 0;
}
