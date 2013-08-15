#define offsetof __builtin_offsetof
#define container_of(ptr,type,member) ({const typeof(((type *)0)->member) *tptr = (ptr); \
	(type *)((char*)tptr - offsetof(type,member));})

struct list_head{
    struct list_head *prev;
    struct list_head *next;
};

#define INIT_LIST_HEAD(x) ({(x).prev = &(x); (x).next = &(x);})
#define list_for_each_entry(v,head,member) for( \
	(v) = container_of((head)->next,typeof(*(v)),member); \
	&((v)->member) != (head); \
	(v) = container_of((v)->member.next,typeof(*(v)),member))
#define list_empty(x) ((x)->next == (x))

extern void list_del(struct list_head *node);
extern void list_add(struct list_head *node,struct list_head *head);
