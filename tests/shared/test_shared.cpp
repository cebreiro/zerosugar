#include <locale>

int main()
{
    std::locale::global(std::locale("kor"));

    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
