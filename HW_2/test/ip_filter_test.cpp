#include <gtest/gtest.h>
#include <openssl/evp.h>

#include <include/ip_filter.hpp>

auto to_hex(
    const unsigned char *const digest,
    const unsigned             size) -> std::string
{
    static const char digits[] = "0123456789abcdef";

    std::string result;

    for (unsigned i = 0; i < size; i++) {
        result += digits[digest[i] / 16];
        result += digits[digest[i] % 16];
    }
    return result;
}

auto computeMD5FromString(
    const std::string &  str,
    unsigned char *const md5,
    unsigned *const      md_len) -> int
{
    const EVP_MD *const md = EVP_md5();
    EVP_MD_CTX *const mdctx = EVP_MD_CTX_new();
    int ret = 0;

    if (nullptr == mdctx)
    {
        ADD_FAILURE() << "Message digest create failed.";
        ret = -1;
        goto finally;
    }

    if (0 == EVP_DigestInit_ex(mdctx, md, nullptr))
    {
        ADD_FAILURE() << "Message digest initialization failed.";
        ret = -2;
        goto finally;
    }

    if (0 == EVP_DigestUpdate(mdctx, str.c_str(), str.length()))
    {
        ADD_FAILURE() << "Message digest update failed.";
        ret = -3;
        goto finally;
    }

    if (0 == EVP_DigestFinal_ex(mdctx, md5, md_len))
    {
        ADD_FAILURE() << "Message digest finalization failed.";
        ret = -4;
        goto finally;
    }

finally:
    EVP_MD_CTX_free(mdctx);

    return ret;
}


// Demonstrate some basic assertions.
TEST(HW_2, ip_filter) {
    std::string encoded;
    std::vector<std::vector<std::string>> ip_pool = stdin_to_vector();
    reverse_lexicographic_sort(ip_pool);

    {
        unsigned char md_value[EVP_MAX_MD_SIZE] = {0};
        unsigned md_len = 0;

        testing::internal::CaptureStdout();

        print(ip_pool);
        auto ip = filter(ip_pool, 1);
        print(ip);
        ip = filter(ip_pool, 46, 70);
        print(ip);
        ip = filter_any(ip_pool, 46);
        print(ip);

        std::string result = testing::internal::GetCapturedStdout();

        EXPECT_EQ(0, computeMD5FromString(result, md_value, &md_len));

        encoded = to_hex(md_value, md_len);
    }

  EXPECT_EQ(encoded, "24e7a7b2270daee89c64d3ca5fb3da1a");
}
