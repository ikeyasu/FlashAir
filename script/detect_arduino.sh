#!/bin/sh

set -ue
cd -- "$(dirname -- "${0}")"

if [ $# -eq 1 ]; then
	echo $1;
	exit 1;
fi

# Mac
if [ -f /Applications/Arduino1.6.app/Contents/MacOS/JavaApplicationStub ]; then
	echo /Applications/Arduino1.6.app/Contents/MacOS/JavaApplicationStub;
	exit 1;
elif [ -f /Applications/Arduino.app/Contents/MacOS/JavaApplicationStub ]; then
	echo /Applications/Arduino.app/Contents/MacOS/JavaApplicationStub;
	exit 1;
else
	echo Could-not-find-Arduino;
	exit 0;
fi
