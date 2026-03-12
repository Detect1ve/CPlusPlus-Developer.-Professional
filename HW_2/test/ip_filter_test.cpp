#include <iterator> // std::back_inserter
#include <span> // std::span
#include <string> // std::string
#include <string_view> // std::string_view

#include <gtest/gtest.h>
#include <boost/hash2/digest.hpp>
#include <boost/hash2/md5.hpp>

#include <capture.hpp>
#include <ip_filter.hpp>

namespace
{
    std::string compute_md5(const std::string_view input)
    {
        boost::hash2::md5_128 hash;

        hash.update(input.data(), input.size());

        return boost::hash2::to_string(hash.result());
    }
} // namespace

TEST(HW2, IpFilter)
{
    auto parse_result = stdin_to_vector();
    std::string encoded;
    std::string result;

    ASSERT_TRUE(parse_result) << "Cannot parse stdin " << parse_result.error().message();

    reverse_lexicographic_sort(parse_result.value());

    {
        StdoutCapture::Begin();

        print(parse_result.value());
        // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        auto ip_result = filter(parse_result.value(), 1);
        ASSERT_TRUE(ip_result) << "Cannot filter " << ip_result.error().message();
        print(ip_result.value());
        ip_result = filter(parse_result.value(), 46, 70);
        ASSERT_TRUE(ip_result) << "Cannot filter " << ip_result.error().message();
        print(ip_result.value());
        ip_result = filter_any(parse_result.value(), 46);
        // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        ASSERT_TRUE(ip_result) << "Cannot filter_any";
        print(ip_result.value());

        result = StdoutCapture::End();

        encoded = compute_md5(result);
    }

    ASSERT_EQ(encoded, "24e7a7b2270daee89c64d3ca5fb3da1a") << result;
}
