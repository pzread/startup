import os
import sys

fna = sys.argv[1]
fnb = sys.argv[2]
off = int(sys.argv[3])

fda = os.open(fna,os.O_WRONLY)
fdb = os.open(fnb,os.O_RDONLY)
os.lseek(fda,off,os.SEEK_SET)
while True:
    data = os.read(fdb,4096)
    if len(data) == 0:
        break

    os.write(fda,data)

os.close(fda)
os.close(fdb)
