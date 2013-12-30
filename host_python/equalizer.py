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
import logging
import ledbar

CHUNK_SIZE = 256
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 44100

PIXELS = 20
LAZY = 0
SYMMETRIC = 0

HISTORY_SIZE = 8
MIN_FREQ = 50
MAX_FREQ = 12000

ATTENUATION = 10**(40/10) # attenuation of 40dB

HUE = 0 # 1 - reddish, 0 - blueish

logging.basicConfig(level='WARNING')

def print_usage():
    print '''\
USAGE:
    %s [-l] [-n number] [-s] [-h]
OPTIONS:
    -l              lazy mode
    -n number       number of controlled boxes
    -s              symmetric mode
    -a number       attenuation in dB (try -a40.0)
    -H number       hue mode: 0 == blue-green (default), 1 == red-blue
    -h --help       show this help
''' % sys.argv[0]

try:
    opts, args = getopt.getopt(sys.argv[1:], 'n:lsha:H:', ['help'])
except getopt.GetoptError:
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
    elif k == '-l':
        LAZY = 1
    elif k == '-s':
        SYMMETRIC = 1
    elif k == '-h' or k == '--help':
        print_usage()
        sys.exit(0)
    elif k == '-a':
        try: v = float(v)
        except ValueError:
            print 'error: attenuation must be float value'
            print_usage()
        ATTENUATION = 10**(v/10)
    elif k == '-H':
        if not v.isdigit():
            print_usage()
            sys.exit(1)
        HUE = int(v)

if LAZY == 1:
    HISTORY_SIZE = 12
if SYMMETRIC == 1:
    EPIXELS = PIXELS / 2
else:
    EPIXELS = PIXELS
# EPIXELS: Effective pixels (for spectrum display)

SAMPLE_SIZE = CHUNK_SIZE*HISTORY_SIZE
FREQ_STEP = float(RATE) / (CHUNK_SIZE * HISTORY_SIZE)
PIXEL_FREQ_RANGE = math.pow(float(MAX_FREQ) / MIN_FREQ, 1.0/EPIXELS)

def with_stream(  fnc ):

    p = pyaudio.PyAudio()

    stream = p.open(format = FORMAT,
                    channels = CHANNELS,
                    rate = RATE,
                    input = True,
                    frames_per_buffer = CHUNK_SIZE)
    try: 
        fnc(stream)
    finally:
        stream.close()
        p.terminate()    


def get_color(volume):
    vol_thres = 200
    if volume <= vol_thres: return (0, 0, 0)
    p = 1-25/(volume-vol_thres)
    if p <= 0: return (0, 0, 0)
    if p >= 1: return (1.0, 1.0, 1.0)
    # Monochromatic mode:
    #p = p * p * p * p * p * p * p
    #return (p, p, 0) # or any other combination
    if LAZY == 1:
        p *= p
    else:
        p *= p * p
    if HUE:
        if p <= 0.4: return (p*2.5,0,0)
        elif p <= 0.7: return (1.0-(p-0.4)*3.33, 0, (p-0.4)*3.33)
        elif p <= 0.9: return (1.0-(p-0.7)*5.0, 0, (p-0.7)*5.0)
        else: return (1.0, (p-0.9)*10.0, (p-0.9)*10.0)
    else:
        if p <= 0.4: return (0, 0, p*2.5)
        elif p <= 0.7: return (0, (p-0.4)*3.33, 1.0-(p-0.4)*3.33)
        elif p <= 0.9: return ((p-0.7)*5.0, 1.0-(p-0.7)*5.0, 0.0)
        else: return (1.0, (p-0.9)*10.0, (p-0.9)*10.0)

def loop( stream ):
    l = ledbar.Ledbar(PIXELS)
    history = []
    
    history_diminish = np.array([[((i+1.0) / HISTORY_SIZE)**2] * CHUNK_SIZE for i in xrange(HISTORY_SIZE)])
    window = np.array([0.5*(1-math.cos(2*math.pi*i/(SAMPLE_SIZE-1))) for i in xrange(SAMPLE_SIZE)])
    work = True
    
    nexttrig = 0
    
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
        
        # obtain input sequence ~~ oohhh what a kind of dimmish magic and windowing
        #TODO: dynamic attenuation based on average power of input signal over timespan, threshold to cut off noise
        x = np.concatenate(history*history_diminish)*window/ATTENUATION
        
        # estimate power spectral desity using autocorelate approach
        psd = np.abs(np.fft.fft(np.correlate(x,x,'same')))[...,np.newaxis]
        # frequencies
        freqs = np.fft.fftfreq(psd.shape[0],1./RATE)[...,np.newaxis]
        # frequency band vector _orthogonal_ to freqs
        bands = np.logspace(np.log2(MIN_FREQ),np.log2(MAX_FREQ),EPIXELS+1,True,2)[np.newaxis,...]
        # integrate energy within bands ~~ oh, oohhh: look at the orthoginality trick
        bands = (freqs>bands[...,:-1]) & (freqs<=bands[...,1:])
        energy = np.round(( bands * psd ).sum(0).squeeze())
        #energy = np.round(( bands * psd / bands.sum(0) ).sum(0).squeeze())

        # write some debug colorfull, very usefull
        ansicolors = ('\033[30;1m%5.0f\033[0m', '\033[33;1m%5.0f\033[0m', '\033[1;31m%5.0f\033[0m')
        sys.stderr.write('\r[%s]     '%','.join( 
            (ansicolors[2] if k>400 else ansicolors[1] if k>200 else ansicolors[0]) %k for k in energy
        ))

        for pixel in xrange(EPIXELS):
            c = get_color(energy[pixel]) # consider using energy**0.5 instead
            if SYMMETRIC == 1:
                l.set_pixel(PIXELS / 2 + pixel, c[0],  c[1], c[2])
                l.set_pixel(PIXELS / 2 - (pixel + 1), c[0],  c[1], c[2])
            else:
                l.set_pixel(pixel, c[0],  c[1], c[2])
        work = l.update()
        # time.sleep(0.05)

if __name__ == '__main__':
    try:
        with_stream(loop)
    except KeyboardInterrupt:
        pass

