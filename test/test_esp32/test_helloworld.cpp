#include <Arduino.h>
#include <unity.h>

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void loop() {}

void test_helloworld()
{
    TEST_ASSERT_TRUE(2 + 3 == 5);
}

void setup()
{
    delay(1000);
    UNITY_BEGIN();

    RUN_TEST(test_helloworld);

    UNITY_END();
}