#include<list.h>

void list_add(struct list_head *node,struct list_head *head){
    node->prev = head;
    node->next = head->next;
    head->next = node;
    node->next->prev = node;
}
void list_del(struct list_head *node){
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->prev = node;
    node->next = node;
}
