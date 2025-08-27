#include <gtest/gtest.h>

#include <capture.hpp>
#include <ip_filter.hpp>
#include <wrapper_boost_algorithm_hex.hpp>
#include <wrapper_boost_uuid_detail_md5.hpp>

namespace
{
    std::string compute_md5(std::string_view input)
    {
        boost::uuids::detail::md5 hash;
        boost::uuids::detail::md5::digest_type digest;
        std::string result;

        hash.process_bytes(input.data(), input.length());
        hash.get_digest(digest);

        auto bytes = std::bit_cast<std::array<unsigned char, sizeof(digest)>>(digest);
        boost::algorithm::hex_lower(bytes, std::back_inserter(result));

        return result;
    }
} // namespace

TEST(HW2, IpFilter)
{
    std::string encoded;
    auto parse_result = stdin_to_vector();
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

        const std::string result = StdoutCapture::End();

        encoded = compute_md5(result);
    }

    ASSERT_EQ(encoded, "24e7a7b2270daee89c64d3ca5fb3da1a");
}
