#!/usr/bin/python
# vim:et:sw=4:ts=4:sts=4

import pyaudio
import struct
import math
import numpy as np
import time
import sys
import getopt
from datetime import datetime

import ledbar

CHUNK_SIZE = 1024
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 44100

PIXELS = 20

SLOW = 0
HISTORY_SIZE = 4
MIN_FREQ = 50
MAX_FREQ = 12000


def print_usage():
    print '''\
USAGE:
    %s [-n number] [-h]
OPTIONS:
    -n number       number of controlled boxes
    -s              slow mode
    -h --help       show this help
''' % sys.argv[0]

try:
    opts, args = getopt.getopt(sys.argv[1:], 'n:sh', ['help'])
except getopt.GetOptError:
    print_usage()
    sys.exit(1)
if len(args):
    print_usage()
    sys.exit(1)
for k, v in opts:
    if k == '-n':
        if not v.isdigit():
            print_usage()
            sys.exit(1)
        PIXELS = int(v)
    elif k == '-s':
        SLOW = 1
    elif k == '-h' or k == '--help':
        print_usage()
        sys.exit(0)

if SLOW == 1:
    HISTORY_SIZE = 12

SAMPLE_SIZE = CHUNK_SIZE*HISTORY_SIZE
FREQ_STEP = float(RATE) / (CHUNK_SIZE * HISTORY_SIZE)
PIXEL_FREQ_RANGE = math.pow(float(MAX_FREQ) / MIN_FREQ, 1.0/PIXELS)


p = pyaudio.PyAudio()

stream = p.open(format = FORMAT,
                channels = CHANNELS,
                rate = RATE,
                input = True,
                frames_per_buffer = CHUNK_SIZE)

def get_color(volume):
    p = 1-15/(volume)
    if p <= 0: return (0, 0, 0)
    # Monochromatic mode:
    # p = p * p * p * p * p * p * p
    # return (0, p/4, p) # or any other combination
    if SLOW == 1:
        p *= p
    else:
        p *= p * p
    if p <= 0.4: return (0, 0, p*2.5)
    elif p <= 0.7: return (0, (p-0.4)*3.33, 1.0-(p-0.4)*3.33)
    elif p <= 0.9: return ((p-0.7)*5.0, 1.0-(p-0.7)*5.0, 0.0)
    else: return (1.0, (p-0.9)*10.0, (p-0.9)*10.0)

l = ledbar.Ledbar(PIXELS)
history = []
window = np.array([0.5*(1-math.cos(2*math.pi*i/(SAMPLE_SIZE-1))) for i in xrange(SAMPLE_SIZE)])
work = True

nexttrig = 0

try:
    while work:
        try: data = stream.read(CHUNK_SIZE)
        except IOError: continue
        nowtrig = datetime.now().microsecond / 50000
        if (nowtrig == nexttrig):
            continue
        else:
            nexttrig = nowtrig
        if len(data) == 0: break
        indata = np.array(struct.unpack('%dh'%CHUNK_SIZE,data))
        history.append(indata)
        if len(history) > HISTORY_SIZE: history.pop(0)
        elif len(history) < HISTORY_SIZE: continue
        fft = np.fft.rfft(np.concatenate(history)*window)
        freq_limit = MIN_FREQ
        freq = 0
        i = 0
        while freq < freq_limit:
            i += 1
            freq += FREQ_STEP
        freq_limit *= PIXEL_FREQ_RANGE
        pixel = 0
        count = 0
        volumes = []
        while pixel < PIXELS:
            total = 0.0
            while freq < freq_limit:
                total += abs(fft[i])**2
                i += 1; count += 1
                freq += FREQ_STEP
            volume = (total/count)**0.5/SAMPLE_SIZE
            volumes.append(volume)
            freq_limit *= PIXEL_FREQ_RANGE
            pixel += 1
            count = 0
        for pixel in xrange(PIXELS):
            c = get_color(volumes[pixel])
            l.set_pixel(pixel, c[0],  c[1], c[2])
        work = l.update()
        # time.sleep(0.05)
finally:
    stream.close()
    p.terminate()
