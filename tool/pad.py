import os
import sys

fn = sys.argv[1]
size = int(sys.argv[2])

fd = os.open(fn,os.O_WRONLY | os.O_APPEND)
fs = os.fstat(fd).st_size
print(fs)
pad = size - fs
assert(pad >= 0)

os.write(fd,b'0' * pad)
os.close(fd)
