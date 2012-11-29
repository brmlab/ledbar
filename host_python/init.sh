#!/bin/bash
mkdir -p /tmp/ledbar
rm -f /tmp/ledbar/rainbow /tmp/ledbar/equalizer /tmp/ledbar/serial

mkfifo /tmp/ledbar/equalizer
mkfifo /tmp/ledbar/rainbow
mkfifo /tmp/ledbar/serial

while true; do
	echo "Starting rainbow"
	./rainbow.py -s > /tmp/ledbar/rainbow
done &

while true; do
	echo "Starting equalizer"
	./equalizer.py -s > /tmp/ledbar/equalizer
done &

while true; do
	echo "Starting serial writer"
	./send_to_serial.py /dev/ttyUSB0 < /tmp/ledbar/serial
done &

echo "Starting multiplexer"
../multiplexer/mux &
wait
