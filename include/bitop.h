#define BITS_PER_LONG (sizeof(long) * 8UL)

extern int test_bit(void *dst,unsigned long off);
extern void set_bit(void *dst,unsigned long off);
extern void clear_bit(void *dst,unsigned long off);
extern unsigned long find_and_set_next_zero_bit(void *dst,
						unsigned long off,
						unsigned long size);

