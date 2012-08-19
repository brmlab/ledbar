#!/usr/bin/python
# vim:et:sw=4:ts=4:sts=4

import sys

from ledbar import Ledbar

PIXELS = 20

def update(t, i):
    offset = float(i)/PIXELS
    time = 0.005*t
    phi = 6*offset+time
    phase = int(phi%6)
    part = phi % 1.0
    inc = part
    dec = 1-part
    if   phase == 0: return (  1, inc,   0)
    elif phase == 1: return (dec,   1,   0)
    elif phase == 2: return (  0,   1, inc)
    elif phase == 3: return (  0, dec,   1)
    elif phase == 4: return (inc,   0,   1)
    elif phase == 5: return (  1,   0, dec)

l = Ledbar(PIXELS)
t = 0
work = True
while work:
    for i in xrange(PIXELS):
        c = update(t, i)
        l.set_pixel(i, c[0], c[1], c[2])
    work = l.update()
    t += 1
