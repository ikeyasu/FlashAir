/* Arduino Sd2Card Library
 * Copyright (C) 2009 by William Greiman
 *
 * This file is part of the Arduino Sd2Card Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino Sd2Card Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef Sd2Card_h
#define Sd2Card_h
/**
 * \file
 * Sd2Card class
 */
#include "AbstructSd2Card.h"
#include "Sd2PinMap.h"
#include "SdInfo.h"

#ifndef SOFTWARE_SPI
// hardware pin defs
/**
 * SD Chip Select pin
 *
 * Warning if this pin is redefined the hardware SS will pin will be enabled
 * as an output by init().  An avr processor will not function as an SPI
 * master unless SS is set to output mode.
 */
/** The default chip select pin for the SD card is SS. */
uint8_t const  SD_CHIP_SELECT_PIN = SS_PIN;
// The following three pins must not be redefined for hardware SPI.
/** SPI Master Out Slave In pin */
uint8_t const  SPI_MOSI_PIN = MOSI_PIN;
/** SPI Master In Slave Out pin */
uint8_t const  SPI_MISO_PIN = MISO_PIN;
/** SPI Clock pin */
uint8_t const  SPI_SCK_PIN = SCK_PIN;
/** optimize loops for hardware SPI */
#ifdef SPCR // not ATtiny
#define OPTIMIZE_HARDWARE_SPI
#endif

#endif

//------------------------------------------------------------------------------
/**
 * \class Sd2Card
 * \brief Raw access to SD and SDHC flash memory cards.
 */
class Sd2Card : public AbstructSd2Card {
  public:
    /** Construct an instance of Sd2Card. */
    Sd2Card(void) : errorCode_(0), inBlock_(0), partialBlockRead_(0), type_(0) {}
#ifndef MEMORY_SAVING
    uint32_t cardSize(void);
    uint8_t erase(uint32_t firstBlock, uint32_t lastBlock);
    uint8_t eraseSingleBlockEnable(void);
    /** \return error data for last error. */
    uint8_t errorData(void);
#endif
    /**
     * \return error code for last error. See Sd2Card.h for a list of error codes.
     */
    uint8_t errorCode(void);
    /**
     * Initialize an SD flash memory card with default clock rate and chip
     * select pin.  See sd2Card::init(uint8_t sckRateID, uint8_t chipSelectPin).
     */
    uint8_t init(void) {
      return init(SPI_FULL_SPEED, SD_CHIP_SELECT_PIN);
    }
    /**
     * Initialize an SD flash memory card with the selected SPI clock rate
     * and the default SD chip select pin.
     * See sd2Card::init(uint8_t sckRateID, uint8_t chipSelectPin).
     */
    uint8_t init(uint8_t sckRateID) {
      return init(sckRateID, SD_CHIP_SELECT_PIN);
    }
    uint8_t init(uint8_t sckRateID, uint8_t chipSelectPin);
#ifndef MEMORY_SAVING
    void partialBlockRead(uint8_t value);
    /** Returns the current value, true or false, for partial block read. */
    uint8_t partialBlockRead(void) const {
      return partialBlockRead_;
    }
#endif
    uint8_t readBlock(uint32_t block, uint8_t* dst);
    uint8_t readData(uint32_t block,
                     uint16_t offset, uint16_t count, uint8_t* dst);
#ifndef MEMORY_SAVING
    /**
     * Read a cards CID register. The CID contains card identification
     * information such as Manufacturer ID, Product name, Product serial
     * number and Manufacturing date. */
    uint8_t readCID(cid_t* cid) {
      return readRegister(CMD10, cid);
    }
    /**
     * Read a cards CSD register. The CSD contains Card-Specific Data that
     * provides information regarding access to the card's contents. */
    uint8_t readCSD(csd_t* csd) {
      return readRegister(CMD9, csd);
    }
#endif
    void readEnd(void);
#if !defined(SOFTWARE_SPI) && defined(SPCR)
    uint8_t setSckRate(uint8_t sckRateID);
#endif
    /** Return the card type: SD V1, SD V2 or SDHC */
    uint8_t type(void) const {
      return type_;
    }
    uint8_t writeBlock(uint32_t blockNumber, const uint8_t* src);
#ifndef MEMORY_SAVING
    uint8_t writeData(const uint8_t* src);
    uint8_t writeStart(uint32_t blockNumber, uint32_t eraseCount);
    uint8_t writeStop(void);
#endif

    uint8_t readExtDataPort(uint8_t mio, uint8_t func, uint16_t addr, uint8_t* dst);
    uint8_t readExtMemory(uint8_t mio, uint8_t func, uint32_t addr, uint16_t count, uint8_t* dst);
    uint8_t writeExtDataPort(uint8_t mio, uint8_t func, uint16_t addr, const uint8_t* src);
    uint8_t writeExtDataPort(uint8_t mio, uint8_t func, uint16_t addr, const uint8_t* src, uint16_t length);
#ifdef MEMORY_SAVING
    uint8_t writeExtMemory(uint8_t mio, uint8_t func, uint32_t addr, uint16_t count, const uint8_t* src);
    uint8_t writeExtMask(uint8_t mio, uint8_t func, uint32_t addr, uint8_t mask, const uint8_t* src);
#endif

  protected:
    uint32_t block_;
    uint8_t chipSelectPin_;
    uint8_t errorCode_;
    uint8_t inBlock_;
    uint16_t offset_;
    uint8_t partialBlockRead_;
    uint8_t status_;
    uint8_t type_;
    // private functions
    uint8_t cardAcmd(uint8_t cmd, uint32_t arg) {
      cardCommand(CMD55, 0);
      return cardCommand(cmd, arg);
    }
    uint8_t cardCommand(uint8_t cmd, uint32_t arg);
    void error(uint8_t code) {
      errorCode_ = code;
    }
    uint8_t readRegister(uint8_t cmd, void* buf);
    uint8_t sendWriteCommand(uint32_t blockNumber, uint32_t eraseCount);
    void chipSelectHigh(void);
    void chipSelectLow(void);
    void type(uint8_t value) {
      type_ = value;
    }
    uint8_t waitNotBusy(uint16_t timeoutMillis);
    uint8_t writeData(uint8_t token, const uint8_t* src);
    uint8_t waitStartBlock(void);

    uint8_t readExt(uint32_t arg, uint8_t* src, uint16_t count);
    uint8_t writeExt(uint32_t arg, const uint8_t* src, uint16_t count);
};
#endif  // Sd2Card_h
