#!/usr/bin/python
# vim:et:sw=4:ts=4:sts=4

import sys
import serial
import getopt

def print_usage():
    print '''\
USAGE:
    %s [-h | [-n number] [-b speed] serial]
OPTIONS:
    serial          write output to serial device
    -b speed        speed of the serial device
    -n number       number of controlled boxes
    -h --help       show this help
''' % sys.argv[0]

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 's:b:n:h', ['help'])
    except getopt.GetOptError:
        print_usage()
        return 1
    speed = 38400
    number = 10
    show_help = False
    for k, v in opts:
        if k == '-n':
            if not v.isdigit():
                print_usage()
                return 1
            number = int(v)
        elif k == '-b':
            if not v.isdigit():
                print_usage()
                return 1
            speed = int(v)
        elif k == '-h' or k == '--help': show_help = True
    if show_help:
        print_usage()
        return 0
    if len(args) != 1:
        print_usage()
        return 1

    try:
        output_stream = serial.Serial(args[0], speed)
    except serial.serialutil.SerialException:
        print 'Could not open the serial device'
        return 1

    try:
        while True:
            data = ''
            to_read = number*3
            while to_read > 0:
                read = sys.stdin.read(to_read)
                if len(read) == 0: break
                to_read -= len(read)
                data += read
            if len(read) == 0: break
            output_stream.write(data)
            output_stream.flush()
    except IOError:
        pass

    return 0

sys.exit(main())
