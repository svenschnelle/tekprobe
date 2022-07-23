#!/usr/bin/python

from hexdump import *
from pyBusPirateLite.I2C import I2C
import sys

i2c = I2C("/dev/ttyACM0")
i2c.speed = '100kHz'
i2c.configure(power=True,pullup=True)

try:
    filename = sys.argv[1]
    size = int(sys.argv[2])
except IndexError:
    raise SystemExit(f"Usage: {sys.argv[0]} <filename> <size>")

file = open(filename, "wb")

for i in range(0, size, 32):
    slave = 0xa0 | ((i >> 8) * 2)
    print(f'slave {slave:02x} offset {i:04x}')
    i2c.write_then_read(2,0, [slave, (i & 0xff)])  # set write register
    file.write(i2c.write_then_read(1,32,[slave|1]))

file.close()
