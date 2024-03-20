#include <number.h>
#include <unity.h>

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void test_thousands_separator()
{
    TEST_ASSERT_EQUAL_STRING("1,000", thousandSeparator(1000, 0).c_str());
    TEST_ASSERT_EQUAL_STRING("100", thousandSeparator(100, 0).c_str());
    TEST_ASSERT_EQUAL_STRING("100.000000", thousandSeparator(100, 6).c_str());
    TEST_ASSERT_EQUAL_STRING("100.00", thousandSeparator(100, 2).c_str());
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_thousands_separator);

    UNITY_END();
}