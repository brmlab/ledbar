#!/usr/bin/python

import sys

from ledbar import Ledbar

PIXELS = 20

rules = {(1,1,1): 0,
    (1,1,0): 0,
    (1,0,1): 0,
    (1,0,0): 1,
    (0,1,1): 1,
    (0,1,0): 1,
    (0,0,1): 1,
    (0,0,0): 0}

iteration = [0]*PIXELS
iteration[PIXELS/2] = 1

def iterate(iteration):
    new = []
    for i in range(len(iteration)):
        if 0 < i < PIXELS-1:
            top = (iteration[i-1], iteration[i], iteration[i+1])
            new.append(rules[top])
        else:
            new.append(0)
    return new

def update(i):
    return (iteration[i], iteration[i], iteration[i])

l = Ledbar(PIXELS)
work = True
t = 0
while work:
    for i in xrange(PIXELS):
        c = update(i)
        l.set_pixel(i, c[0], c[1], c[2])
    work = l.update()
    t += 1
    if not (t % 50):
        iteration = iterate(iteration)
