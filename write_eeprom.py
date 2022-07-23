#!/usr/bin/python

from pyBusPirateLite.I2C import I2C
import sys
import time
i2c = I2C("/dev/ttyACM0")
i2c.speed = '5kHz'
i2c.configure(power=True,pullup=True)

try:
    file = open(sys.argv[1],"rb")
except IndexError:
    raise SystemError(f"Usage: {sys.argv[0]} <filename>")

bytes = file.read()
i = 0
for c in bytes:
    slave = 0xa0 + (i >> 8) * 2
    print(f'\r{slave:02x} {i:3d}={c:02x}', end='')
    i2c.write_then_read(3, 0, [slave, i & 0xff, c])
    time.sleep(0.01)
    i = i + 1
print('\ndone')
file.close()
