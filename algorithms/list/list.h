/*************************************************************************
    > File Name: list.h
    > Author: wangjx
 ************************************************************************/
#ifndef __LIST_H__
#define __LIST_H__
#include <stdio.h>
#include <stdlib.h>
struct list_head {
	struct list_head *prev, *next;
	int val;
};
#define LIST_HEAD(x)	struct list_head x = {.prev = &x, .next = &x, .val = 0}
int list_add_tail(struct list_head *head, int val)
{
	struct list_head *node = calloc(1, sizeof(struct list_head));
	node->val = val;

	struct list_head *prev = head->prev;

	prev->next = node;
	node->prev = prev;

	head->prev = node;
	node->next = head;
	return 0;
}
int list_del_tail(struct list_head *head)
{
	struct list_head *prev = head->prev;
	if (prev == head)
		return -1;
	int val = prev->val;
	prev->prev->next = head;
	head->prev = prev->prev;
	free(prev);

	return val;
}
int list_del_head(struct list_head *head)
{
	struct list_head *next = head->next;
	if (next == head)
		return -1;
	int val = next->val;
	next->next->prev = head;
	head->next = next->next;
	free(next);

	return val;
}
#define stack_push(x,y) list_add_tail(x, y)
#define stack_pop(x) list_del_tail(x)
#define queue_in(x,y) list_add_tail(x, y)
#define queue_out(x) list_del_head(x)
struct list_head *list_find(struct list_head *head, int val)
{
	struct list_head *tmp = NULL;
	for (tmp = head->next; tmp != head; tmp = tmp->next) {
		if (tmp->val == val) {
			printf("find %d success\n", tmp->val);
			return tmp;
		}
	}
	return NULL;
}
int list_del(struct list_head *head, int val)
{
	struct list_head *tmp = list_find(head, val);
	if (tmp) {
		tmp->prev->next = tmp->next;
		tmp->next->prev = tmp->prev;
		free(tmp);
		return 0;
	}
	return -1;
}
void list_show(struct list_head *head)
{
	struct list_head *tmp = NULL;
	for (tmp = head->next; tmp != head; tmp = tmp->next) {
		printf("%d ", tmp->val);
	}
	printf("\n");
}
#endif /*__LIST_H__*/
