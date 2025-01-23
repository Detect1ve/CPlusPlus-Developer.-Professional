#include <algorithm>

#include <gtest/gtest.h>
#include <openssl/evp.h>

#include <ip_filter.hpp>

constexpr auto to_hex(std::span<const unsigned char> digest) -> string
{
    static const std::array digits = std::to_array<const char>("0123456789abcdef");
    string result;
    result.reserve(digest.size() * 2);

    std::ranges::for_each(digest, [&result](const unsigned char byte)
    {
        result += digits[byte / 16];
        result += digits[byte % 16];
    });

    return result;
}

auto computeMD5FromString(
    std::string_view     str,
    unsigned char *const md5,
    unsigned *const      md_len) -> int
{
    const EVP_MD *const md = EVP_md5();
    struct MD_CTX_Deleter {
        void operator()(EVP_MD_CTX* ctx) const
        {
            if (ctx != nullptr)
            {
                EVP_MD_CTX_free(ctx);
            }
        }
    };
    std::unique_ptr<EVP_MD_CTX, MD_CTX_Deleter> mdctx(EVP_MD_CTX_new());
    int ret = 0;

    if (nullptr == mdctx)
    {
        ADD_FAILURE() << "Message digest create failed.";
        ret = -1;

        return ret;
    }

    if (0 == EVP_DigestInit_ex(mdctx.get(), md, nullptr))
    {
        ADD_FAILURE() << "Message digest initialization failed.";
        ret = -2;

        return ret;
    }

    if (0 == EVP_DigestUpdate(mdctx.get(), str.data(), str.length()))
    {
        ADD_FAILURE() << "Message digest update failed.";
        ret = -3;

        return ret;
    }

    if (0 == EVP_DigestFinal_ex(mdctx.get(), md5, md_len))
    {
        ADD_FAILURE() << "Message digest finalization failed.";
        ret = -4;

        return ret;
    }

    return ret;
}

TEST(HW_2, ip_filter) {
    string encoded;
    auto parse_result = stdin_to_vector();
    ASSERT_TRUE(parse_result) << "Cannot parse stdin " << parse_result.error().message();

    reverse_lexicographic_sort(parse_result.value());

    {
        unsigned char md_value[EVP_MAX_MD_SIZE] = {0};
        unsigned md_len = 0;

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

        ASSERT_EQ(0, computeMD5FromString(result, md_value, &md_len));

        encoded = to_hex(std::span<const unsigned char>(md_value, md_len));
    }

    EXPECT_EQ(encoded, "24e7a7b2270daee89c64d3ca5fb3da1a");
}
