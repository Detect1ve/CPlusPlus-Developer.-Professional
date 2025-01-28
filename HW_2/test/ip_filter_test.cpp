#include <gtest/gtest.h>
#include <openssl/evp.h>

#include <include/ip_filter.hpp>
#include <ostream>

// void print_MD5(unsigned char *md, long size = MD5_DIGEST_LENGTH){
//     std::cout << md << std::endl;
//     for (int i = 0; i < size; i++){
//         std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)md[i];
//     }
//     std::cout << std::endl;
// }

// // Function to compute and print MD5 hash of a given string
// void computeMD5FromString(const std::string &str){
//     unsigned char result[MD5_DIGEST_LENGTH];
//     MD5((unsigned char *)str.c_str(), str.length(), result);

//     // std::cout << "MD5 of '" << str << "' : ";
//     print_MD5(result);
// }


auto computeMD5FromString(
    const std::string &  str,
    unsigned char *const md5,
    unsigned int *const  md_len) -> int
{
    const EVP_MD *const md = EVP_md5();
    EVP_MD_CTX *const mdctx = EVP_MD_CTX_new();
    int ret = 0;

	ADD_FAILURE() << "TTEST";
EXPECT_EQ(nullptr, mdctx);
    if (nullptr == mdctx)
    {
        std::cerr << "Message digest create failed." << std::endl;
        ret = -1;
        goto finally;
    }

    if (0 == EVP_DigestInit_ex(mdctx, md, nullptr))
    {
        std::cerr << "Message digest initialization failed." << std::endl;
        ret = -2;
        goto finally;
    }

    if (0 == EVP_DigestUpdate(mdctx, str.c_str(), str.length()))
    {
        std::cerr << "Message digest update failed." << std::endl;
        ret = -3;
        goto finally;
    }

    if (0 == EVP_DigestFinal_ex(mdctx, md5, md_len))
    {
        std::cerr << "Message digest finalization failed." << std::endl;
        ret = -4;
        goto finally;
    }

finally:
    EVP_MD_CTX_free(mdctx);

    return ret;
}


// Demonstrate some basic assertions.
TEST(HW_2, ip_filter) {
    std::vector<std::vector<std::string>> ip_pool = stdin_to_vector();
    reverse_lexicographic_sort(ip_pool);

    {
        unsigned char md_value[EVP_MAX_MD_SIZE] = {0};
        unsigned int md_len = 0;

        testing::internal::CaptureStdout();

        print(ip_pool);
        auto ip = filter(ip_pool, 1);
        print(ip);
        ip = filter(ip_pool, 46, 70);
        print(ip);
        ip = filter_any(ip_pool, 46);
        print(ip);

        std::string result = testing::internal::GetCapturedStdout();
        computeMD5FromString(result, md_value, &md_len);
    }

    // 24e7a7b2270daee89c64d3ca5fb3da1a
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_NE(7 * 6, 42);
}
