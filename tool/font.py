import os

fd = os.open('in.fnt',os.O_RDONLY)
buf = os.read(fd,4096)

li = []
for i in range(4096):
    v = ord(buf[i])
    li.append(str(v))

os.close(fd)

print 'static unsigned char font_map[4096] = {%s};'%','.join(li)

