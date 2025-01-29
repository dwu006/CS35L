#!/usr/bin/env python3

import argparse, random, sys

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-e", "--echo", nargs="*")
    parser.add_argument("-i", "--input-range", metavar="LO-HI", type=str)
    parser.add_argument("-n", "--head-count", metavar="COUNT", type=int)
    parser.add_argument("-r", "--repeat", action="store_true")
    parser.add_argument("filename", nargs="?")

    args = parser.parse_args()
    
    buffer = []
    
    if args.echo is not None:
        buffer = args.echo.copy()
    elif args.input_range is not None:
        input_range = args.input_range
        try:
            lo_str, hi_str = input_range.split('-', 1)
            lo = int(lo_str)
            hi = int(hi_str)
        except ValueError:
            parser.error("invalid input range: '{}'".format(input_range))
        if lo > hi:
            parser.error("invalid input range: '{}'".format(input_range))
        buffer = [str(x) for x in range(lo, hi + 1)]
    elif args.filename is not None:
        if args.filename == '-':
            buffer = sys.stdin.read().splitlines()
        else:
            try:
                with open(args.filename, 'r') as f:
                    buffer = f.read().splitlines()
            except IOError as e:
                parser.error("{}: {}".format(e.strerror, args.filename))
    else:
        buffer = sys.stdin.read().splitlines()
    
    if args.repeat and not buffer:
        sys.exit(0)
    
    if not args.repeat:
        random.shuffle(buffer)
        if args.head_count is not None:
            output = buffer[:args.head_count]
        else:
            output = buffer
        for line in output:
            print(line)
    else:
        if args.head_count is not None:
            for _ in range(args.head_count):
                print(random.choice(buffer))
        else:
            while True:
                print(random.choice(buffer))

if __name__ == "__main__":
    main()
