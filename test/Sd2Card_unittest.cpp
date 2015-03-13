#include "gtest/gtest.h"
#include "Sd2Card.h"

using ::testing::Return;
using ::testing::InSequence;
using ::testing::_;
using ::testing::AnyNumber;

void expectCardCommand(ArduinoMock* arduinoMock, SPIMock* spiMock,
    uint8_t cmd, uint32_t arg, uint8_t status) {
  InSequence sequential_test;
  // readEnd
  EXPECT_CALL(*spiMock, spiReceive()).Times(AnyNumber());

  EXPECT_CALL(*arduinoMock, digitalWrite(0, 0)).WillOnce(Return());

  // waitNotBusy
  EXPECT_CALL(*arduinoMock, millis()).WillOnce(Return(0));
  EXPECT_CALL(*spiMock, spiReceive()).WillOnce(Return(0xFF));

  EXPECT_CALL(*spiMock, spiSend(cmd | 0x40)).Times(1);

  for (int8_t s = 24; s >= 0; s -= 8) {
    EXPECT_CALL(*spiMock, spiSend(arg >> s)).Times(1);
  }
  uint8_t crc = 0xFF;
  if (cmd == CMD0) crc = 0x95;
  if (cmd == CMD8) crc = 0x87;
  EXPECT_CALL(*spiMock, spiSend(crc)).Times(1);

  EXPECT_CALL(*spiMock, spiReceive()).WillRepeatedly(Return(status));
}

TEST(Sd2Card, cardCommand) {
  ArduinoMock* arduinoMock = arduinoMockInstance();
  SPIMock* spiMock = spiMockInstance();
  Sd2Card* card = new Sd2Card();

  expectCardCommand(arduinoMock, spiMock, CMD0, 0, 0);
  ASSERT_EQ(0, card->cardCommand(CMD0, 0));

  releaseSpiMock();
  releaseArduinoMock();
}

TEST(Sd2Card, cardCommandStatus0xFF) {
  ArduinoMock* arduinoMock = arduinoMockInstance();
  SPIMock* spiMock = spiMockInstance();
  Sd2Card* card = new Sd2Card();

  expectCardCommand(arduinoMock, spiMock, CMD0, 0, 0xFF);
  ASSERT_EQ(0xFF, card->cardCommand(CMD0, 0));

  releaseSpiMock();
  releaseArduinoMock();
}

TEST(Sd2Card, cardCommandCMD48) {
  ArduinoMock* arduinoMock = arduinoMockInstance();
  SPIMock* spiMock = spiMockInstance();
  Sd2Card* card = new Sd2Card();

  expectCardCommand(arduinoMock, spiMock, CMD48, 0, 0);
  ASSERT_EQ(0, card->cardCommand(CMD48, 0));

  releaseSpiMock();
  releaseArduinoMock();
}


void EXPECT_CALL_SPISEND(SPIMock* mock, uint8_t* expected_array,
    uint32_t array_len) {
  for (unsigned long i = 0; i < array_len; i++) {
    EXPECT_CALL(*mock, spiSend(expected_array[i])).Times(1);
  }
}

TEST(Sd2Card, writeExtDataPort) {
  ArduinoMock* arduinoMock = arduinoMockInstance();
  SPIMock* spiMock = spiMockInstance();
  Sd2Card* card = new Sd2Card();
  uint8_t buf[3] = {0, 0, 0};

  EXPECT_CALL(*arduinoMock, digitalWrite(_, _)).Times(AnyNumber());
  EXPECT_CALL(*arduinoMock, millis()).WillRepeatedly(Return(0));
  EXPECT_CALL(*spiMock, spiSend(_)).Times(AnyNumber());
  EXPECT_CALL(*spiMock, spiReceive()).WillRepeatedly(Return(0xFF));

  card->writeExtDataPort(1, 1, 0, buf, 3);

  releaseSpiMock();
  releaseArduinoMock();
}

TEST(Sd2Card, waitStartBlock) {
  ArduinoMock* arduinoMock = arduinoMockInstance();
  SPIMock* spiMock = spiMockInstance();
  Sd2Card* card = new Sd2Card();

  // wait for star block token
  EXPECT_CALL(*arduinoMock, millis()).WillOnce(Return(0));
  EXPECT_CALL(*spiMock, spiReceive()).WillOnce(Return(DATA_START_BLOCK));

  card->waitStartBlock();

  releaseSpiMock();
  releaseArduinoMock();
}

TEST(Sd2Card, readExt) {
  InSequence sequential_test;
  ArduinoMock* arduinoMock = arduinoMockInstance();
  SPIMock* spiMock = spiMockInstance();
  Sd2Card* card = new Sd2Card();
  uint8_t buffer[10];

  expectCardCommand(arduinoMock, spiMock, CMD48, 0, 0);

  // wait for star block token
  EXPECT_CALL(*arduinoMock, millis()).WillOnce(Return(0));
  EXPECT_CALL(*spiMock, spiReceive()).WillOnce(Return(DATA_START_BLOCK));

  EXPECT_CALL(*spiMock, spiReceive()).Times(10).WillRepeatedly(Return(1));
  EXPECT_CALL(*spiMock, spiReceive()).Times(514-10).WillRepeatedly(Return(0));
  EXPECT_CALL(*arduinoMock, digitalWrite(_, 1)).WillOnce(Return());
  EXPECT_CALL(*spiMock, spiSend(0xFF)).Times(1);

  card->readExt(0, buffer, 10);

  releaseSpiMock();
  releaseArduinoMock();
}
