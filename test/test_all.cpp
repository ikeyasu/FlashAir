#include <Arduino.h>
#include "Serial.h"

#include "CommandQueue_unittest.cpp"
#include "Sd2Card_unittest.cpp"

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
