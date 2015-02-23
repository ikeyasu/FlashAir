ARDUINO = `./script/detect_arduino.sh`

all: *.ino *.h *.cpp
	$(ARDUINO) --verify --board arduino:avr:uno `pwd`/FlashAirLib.ino

upload: *.ino *.h *.cpp
	$(ARDUINO) --upload --board arduino:avr:uno `pwd`/FlashAirLib.ino
