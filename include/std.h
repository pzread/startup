extern void memset(void *dst,char value,unsigned long size);
extern int memcmp(void *dst,void *src,unsigned long size);
extern void memcpy(void *dst,void *src,unsigned long size);

extern void sprintf(char *str,char *fmt,unsigned long *args);

extern void video_drawtext(int x,int y,char *str);
extern void log(char *msg);

