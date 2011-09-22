#!/usr/bin/python
# vim:et:sw=4:ts=4:sts=4

import sys
import time

class Ledbar:

    def __init__(self, boxes=10, secs_per_frame=0.025):
        self.boxes = boxes
        self.secs_per_frame = secs_per_frame
        self.last_update = time.time()
        self.pixels = []
        for i in xrange(boxes):
            self.pixels.append([0, 0, 0])

    def set_pixel(self, pixel, red, green, blue):
        self.set_red(pixel, red)
        self.set_green(pixel, green)
        self.set_blue(pixel, blue)

    def set_red(self, pixel, red):
        if red < 0.0 or red > 1.0: raise ValueError('red has to be between 0.0 and 1.0')
        self.pixels[pixel][0] = int(red*255.99)

    def set_green(self, pixel, green):
        if green < 0.0 or green > 1.0: raise ValueError('green has to be between 0.0 and 1.0')
        self.pixels[pixel][1] = int(green*255.99)

    def set_blue(self, pixel, blue):
        if blue < 0.0 or blue > 1.0: raise ValueError('blue has to be between 0.0 and 1.0')
        self.pixels[pixel][2] = int(blue*255.99)

    def echo(self, s, no_newline=False):
        sys.stderr.write(str(s) + ('' if no_newline else '\n'))

    def update(self):
        now = time.time()
        delta = now - self.last_update
        if delta < self.secs_per_frame:
            time.sleep(self.secs_per_frame - delta)
        try:
            for p in self.pixels:
                for c in p:
                    sys.stdout.write(chr(c))
            sys.stdout.flush()
        except IOError:
            return False
        self.last_update += self.secs_per_frame
        return True
