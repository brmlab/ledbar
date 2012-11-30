#!/bin/bash
mkdir -p /tmp/ledbar
rm -f /tmp/ledbar/rainbow /tmp/ledbar/equalizer /tmp/ledbar/serial

mkfifo /tmp/ledbar/equalizer
mkfifo /tmp/ledbar/rainbow
mkfifo /tmp/ledbar/serial


cd host_python
while true; do
	echo "Starting rainbow"
	./rainbow.py -s > /tmp/ledbar/rainbow 2>/dev/null
done &

while true; do
	echo "Starting equalizer"
	./equalizer.py -s > /tmp/ledbar/equalizer 2>/dev/null
done &

while true; do
	echo "Starting serial writer"
	./send_to_serial.py /dev/ttyUSB0 < /tmp/ledbar/serial 2>/dev/null
done &

echo "Starting multiplexer"
../mux/mux 
wait
