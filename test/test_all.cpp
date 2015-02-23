//#include "src/gtest_main.cc"
#include "utility/AbstructSd2Card.h"

#include "Arduino.h"
#include "Serial.h"

#include "CommandQueue_unittest.cpp"

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
