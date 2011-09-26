#!/usr/bin/python
# vim:et:sw=4:ts=4:sts=4

import sys
import getopt
import pygame

def print_usage():
    print '''\
USAGE:
    %s [-n number] [-h]
OPTIONS:
    -n number       number of controlled boxes
    -h --help       show this help
''' % sys.argv[0]

def read_byte():
    r = sys.stdin.read(1)
    if len(r) == 0: raise EOFError
    return ord(r)

def write_byte(b):
    sys.stdout.write(chr(b))

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'n:h', ['help'])
    except getopt.GetOptError:
        print_usage()
        return 1
    if len(args):
        print_usage()
        return 1
    number = 10
    show_help = False
    for k, v in opts:
        if k == '-n':
            if not v.isdigit():
                print_usage()
                return 1
            number = int(v)
        elif k == '-h' or k == '--help': show_help = True
    if show_help:
        print_usage()
        return 0

    pygame.init()
    screen_size = [800, 600]
    screen = pygame.display.set_mode(screen_size)
    pygame.display.set_caption("ledbar demo viewer")
    offset = 5
    pixel_width = (screen_size[0]-offset) / number
    try:
        exit = False
        while not exit:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    exit = True
                    continue
            screen.fill([0, 0, 0])
            for i in xrange(number):
                r = read_byte()
                g = read_byte()
                b = read_byte()
                pygame.draw.rect(screen, [r, g, b], [pixel_width*i, 0, pixel_width-offset, pixel_width-offset])
                write_byte(r); write_byte(g); write_byte(b)
            sys.stdout.flush()
            pygame.display.flip()
    except EOFError:
        pass
    finally:
        pygame.quit()

    return 0

sys.exit(main())
