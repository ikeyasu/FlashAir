all: *.ino *.h *.ino
	/Applications/Arduino1.6.app/Contents/MacOS/JavaApplicationStub --verify --board arduino:avr:uno `pwd`/FlashAirLib.ino
