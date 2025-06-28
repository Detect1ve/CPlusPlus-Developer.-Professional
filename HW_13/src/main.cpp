#include <cstddef> // std::size_t
#include <cstdio> // stderr
#include <exception> // std::exception
#if defined(__clang__)\
 || defined(_MSC_VER) && !defined(__clang__) && !defined(__INTEL_COMPILER)\
 || __GNUC__ < 14
#include <span>
#endif
#include <string> // std::string

#include <custom_print.hpp>
#include <mlp.hpp>

namespace
{
    void printUsage(const char *const programName)
    {
        cp::println("Usage: {} <test_data_path> <model_dir>", programName);
        cp::println("Example: {} test.csv model", programName);
    }
} // namespace

int main(
    const int   argc,
    const char* argv[])
{
    const std::span<const char*> args(argv, static_cast<std::size_t>(argc));
    int ret = 0;

    try
    {
        if (args.size() != 3)
        {
            cp::println(stderr, "Error: Invalid number of arguments!");
            printUsage(args[0]);
            ret = -1;

            return ret;
        }

        const std::string test_data_path(args[1]);
        const std::string model_dir(args[2]);

        const std::string w1_path = model_dir + "/w1.txt";
        const std::string w2_path = model_dir + "/w2.txt";

        MLP model(w1_path, w2_path);

        const float accuracy = model.evaluate(test_data_path);

        cp::println("{:.3f}", accuracy);

    }
    catch (const std::exception& e)
    {
        cp::safe_error(e.what());
        ret = -2;
    }
    catch (...)
    {
        cp::safe_error(nullptr);
        ret = -3;
    }

    return ret;
}
