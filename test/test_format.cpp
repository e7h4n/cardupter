#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <number.h>

TEST(number, thousands_separator) {
    EXPECT_EQ("1,000", thousandSeparator(1000, 0));
    EXPECT_EQ("100", thousandSeparator(100, 0));
    EXPECT_EQ("100.000000", thousandSeparator(100, 6));
    EXPECT_EQ("100.00", thousandSeparator(100, 2));
}

#if defined(ARDUINO)

#include <Arduino.h>

void setup() {
    Serial.begin(115200);

    ::testing::InitGoogleMock();
}

void loop() {
    RUN_ALL_TESTS();

    delay(1000);
}

#else

int main(int argc, char **argv) {
    ::testing::InitGoogleMock(&argc, argv);

    RUN_ALL_TESTS();

    return 0;
}

#endif