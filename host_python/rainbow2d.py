#!/usr/bin/python

from __future__ import print_function
from ledbar import Ledbar
import math, time
import sys

WIDTH = 4
HEIGHT = 5
PIXELS = 20

def lint(x, points, values):
  assert(len(points) == len(values))

  for i in range(len(points)-1):
    if x > points[i+1]:
      continue
    width = points[i+1] - points[i]
    assert(width > 0)

    t = (x - points[i]) / width
    t_1 = 1.0 - t
    return values[i]*t_1 + values[i+1]*t 

  return values[len(values)-1]

# Correction values
corr_r_p = [0.0, 0.5, 0.7, 0.9, 1.0]
corr_r_v = [0.0, 0.05, 0.15, 0.45, 1.0]
corr_g_p = [0.0, 0.55, 0.9, 1.0]
corr_g_v = [0.0, 0.08, 0.2, 1.0]
corr_b_p = [0.0, 0.6, 0.75, 1.0]
corr_b_v = [0.0, 0.13, 0.18, 1.0]

def set_pixel_2d(bar, x, y, r, g, b):
  bar.set_pixel(WIDTH - x - 1 + y*WIDTH, r, g, b)

def combine(*fncs):
    return lambda *args: reduce(
        id,
        reduce(
            lambda acc,fnc:(fnc(*acc),), 
            reversed(fncs), 
            args)
    )

def flip(f):
    return lambda *a: f(*reversed(a))

def cdist(center, point, scale):
  from operator import add,sub
  from math import sqrt
  from functools import partial
  return scale*sqrt(reduce(add, map(combine(partial(flip(pow),2),sub),center,point)))

l = Ledbar(PIXELS)

M = max(WIDTH, HEIGHT)
center_b = [0,0]
work = True
t = 0
while work:
  center_r = [math.sin(t)*(M/2)+WIDTH/2-1,     math.cos(t)*(M/2)+HEIGHT/2-1]
  center_g = [math.sin(t*0.53234)*(M/2)+WIDTH/2-1, HEIGHT/2-1]
  print ("red: ", center_r[0], center_r[1], "  green: ", center_g[0], center_g[1], end = "     \r", file = sys.stderr)
  for x in range(WIDTH):
    for y in range(HEIGHT):
      r = cdist(center_r, [x,y], 0.2)
      r = lint(r, corr_r_p, corr_r_v)
      g = cdist(center_g, [x,y], 0.2)
      g = lint(g, corr_g_p, corr_g_v)
      set_pixel_2d(l, x, y, r, g, 0)
  work = l.update()
  t += 0.01
