#include <lcthw/list.h>
#include <lcthw/dbg.h>

List *List_create()
{
    return calloc(1, sizeof(List));
}


void List_clear_destroy(List * list)
{
    check(list, "List is NULL");

    int actual_count = 0;
    LIST_FOREACH(list, first, next, cur) {
        if (cur->prev) {
            free(cur->prev);
            free(cur->value);
        }
    }

    free(list->last);
    free(list);

    check(actual_count == list->count, "Actual count is not the same as list->count");

error:
    return;
}

void List_push(List * list, void *value)
{
    check(list, "List is NULL");

    int old_count = list->count;  // Capture count before modification

    ListNode *node = calloc(1, sizeof(ListNode));
    check_mem(node);

    node->value = value;

    if (list->last == NULL) {
        list->first = node;
        list->last = node;
    } else {
        list->last->next = node;
        node->prev = list->last;
        list->last = node;
    }

    list->count++;

    check(list->count == old_count + 1, "Count did not increase by one.");
    check(((list->count == 0 && list->first == NULL && list->last == NULL) ||
           (list->count > 0 && list->first != NULL && list->last != NULL)), "List is in an invalid state.");

error:
    return;
}

void *List_pop(List * list)
{
    ListNode *node = list->last;
    return node != NULL ? List_remove(list, node) : NULL;
}

void List_unshift(List * list, void *value)
{
    ListNode *node = calloc(1, sizeof(ListNode));
    check_mem(node);

    node->value = value;

    if (list->first == NULL) {
        list->first = node;
        list->last = node;
    } else {
        node->next = list->first;
        list->first->prev = node;
        list->first = node;
    }

    list->count++;

error:
    return;
}

void *List_shift(List * list)
{
    ListNode *node = list->first;
    return node != NULL ? List_remove(list, node) : NULL;
}

void *List_remove(List * list, ListNode * node)
{
    void *result = NULL;

    check(list->first && list->last, "List is empty.");
    check(node, "node can't be NULL");

    if (node == list->first && node == list->last) {
        list->first = NULL;
        list->last = NULL;
    } else if (node == list->first) {
        list->first = node->next;
        check(list->first != NULL,
                "Invalid list, somehow got a first that is NULL.");
        list->first->prev = NULL;
    } else if (node == list->last) {
        list->last = node->prev;
        check(list->last != NULL,
                "Invalid list, somehow got a next that is NULL.");
        list->last->next = NULL;
    } else {
        ListNode *after = node->next;
        ListNode *before = node->prev;
        after->prev = before;
        before->next = after;
    }

    list->count--;
    result = node->value;
    free(node);

error:
    return result;
}