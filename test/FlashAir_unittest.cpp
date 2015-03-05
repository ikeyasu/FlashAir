#include "gtest/gtest.h"
#include "FlashAir.h"

using ::testing::Return;
using ::testing::InSequence;
using ::testing::_;
using ::testing::AnyNumber;

TEST(FlashAir, constructor) {
  ArduinoMock* arduinoMock = arduinoMockInstance();
  SPIMock* spiMock = spiMockInstance();

  EXPECT_CALL(*arduinoMock, digitalWrite(_, _)).Times(AnyNumber());
  EXPECT_CALL(*arduinoMock, millis()).WillRepeatedly(Return(0));
  EXPECT_CALL(*spiMock, spiSend(_)).Times(AnyNumber());
  EXPECT_CALL(*spiMock, spiReceive()).WillRepeatedly(Return(0xFF));

  FlashAir* flashair = new FlashAir(1);

  releaseSpiMock();
  releaseArduinoMock();
}
