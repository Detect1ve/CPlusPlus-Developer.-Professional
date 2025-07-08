#include <algorithm>

#include <boost/algorithm/hex.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <gtest/gtest.h>

#include <ip_filter.hpp>

auto compute_md5(std::string_view input) -> string
{
    boost::uuids::detail::md5 hash;
    boost::uuids::detail::md5::digest_type digest;
    string result;

    hash.process_bytes(input.data(), input.length());
    hash.get_digest(digest);

    const auto *const char_digest = reinterpret_cast<const char*>(&digest);

    boost::algorithm::hex_lower(char_digest, char_digest + sizeof(digest),
        std::back_inserter(result));

    return result;
}

TEST(HW_2, ip_filter) {
    string encoded;
    auto parse_result = stdin_to_vector();
    ASSERT_TRUE(parse_result) << "Cannot parse stdin " << parse_result.error().message();

    reverse_lexicographic_sort(parse_result.value());

    {
        testing::internal::CaptureStdout();

        print(parse_result.value());
        auto ip = filter(parse_result.value(), 1);
        ASSERT_TRUE(ip) << "Cannot filter " << ip.error().message();
        print(ip.value());
        ip = filter(parse_result.value(), 46, 70);
        ASSERT_TRUE(ip) << "Cannot filter " << ip.error().message();
        print(ip.value());
        ip = filter_any(parse_result.value(), 46);
        ASSERT_TRUE(ip) << "Cannot filter_any";
        print(ip.value());

        string result = testing::internal::GetCapturedStdout();

        encoded = compute_md5(result);
    }

    EXPECT_EQ(encoded, "24e7a7b2270daee89c64d3ca5fb3da1a");
}
