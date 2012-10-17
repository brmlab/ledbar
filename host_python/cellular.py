#!/usr/bin/python
"""
An elementary 2D celluar autonoma implementation for the ledbar in brmlab.
For some fun rules, try:
    30: near-random behavior
    22: gives a symmetric triangle pattern.  It just looks like splitting cells and gets
empty quickly, though.  
    142: neat waves
    73: provides a downwards pattern with some fixed columns.  
    51: likes to blink.

The color mode encodes individual bits into, well, colors.  Not too exciting,
but sure more colorful.

You can choose between a single pixel or a random starting row.

Possible fun stuff:  Automatically pick new rules, detect patterns and restart.
Also, implement grayscare totalistic autonoma.
"""


import sys
import random

from ledbar import Ledbar

PIXELS = 20
PIXEL_MODE = ('bw', 'color')[0]
START = ('single', 'random')[0]
RULE = 30

WIDTH = PIXELS
if PIXEL_MODE == 'color': WIDTH *= 3

def bits(num, align=8):
    for i in range(align)[::-1]:
        yield bool(num & (1 << i))

rules = dict(zip(((1,1,1), (1,1,0), (1,0,1), (1,0,0), (0,1,1), (0,1,0), (0,0,1), (0,0,0)), bits(RULE)))

iteration = [0]*WIDTH
if START == 'single':
    iteration[WIDTH//2] = 1
elif START == 'random':
    iteration = list(random.randint(0, 1) for i in iteration)

def iterate(iteration):
    new = []
    iteration.insert(0, 0)
    iteration.append(0)
    for i in xrange(len(iteration)):
        if 0 < i < len(iteration)-1:
            top = (iteration[i-1], iteration[i], iteration[i+1])
            new.append(rules[top])
        else:
            new.append(0)
    return new

def update(i):
    visible = iteration[(len(iteration)//2)-(WIDTH//2):(len(iteration)//2)+(WIDTH//2)]
    if PIXEL_MODE == 'bw':
        return (visible[i], visible[i], visible[i])
    elif PIXEL_MODE == 'color':
        return (visible[3*i], visible[3*i+1], visible[3*i+2])

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
