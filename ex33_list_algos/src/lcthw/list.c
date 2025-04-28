#include <lcthw/list.h>
#include <lcthw/dbg.h>

List *List_create()
{
    return calloc(1, sizeof(List));
}

void List_clear(List * list)
{
    LIST_FOREACH(list, first, next, cur) {
        free(cur->value);
    }
}

void List_destroy(List * list)
{
    LIST_FOREACH(list, first, next, cur) {
        if (cur->prev) {
            free(cur->prev);
        }
    }

    free(list->last);
    free(list);
}

void List_clear_destroy(List * list)
{
    check(list, "List is NULL");

    LIST_FOREACH(list, first, next, cur) {
        if (cur->prev) {
            free(cur->prev);
            free(cur->value);
        }
    }

    free(list->last);
    free(list);
    check_mem(list);

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

void List_print(List *list) {
    check(list->first && list->last, "List is empty.");
    ListNode *current = list->first;
    while (current) {
        printf("%s <-> ", (char *) current->value);
        current = current->next;
    }
    printf("NULL\n");

error:
    return;
}

void List_join(List *list1, List *list2)
{
    check(list1, "List1 is NULL");
    check(list2, "List2 is NULL");
    
    list1->last->next = list2->first;
    list2->first->prev = list1->last;

    list1->last = list2->last;
    list1->count += list2->count;

error:
    return;
}


// Inefficient implementation
// void *List_split(List *list, int index)
// {
//     check(list->first && list->last, "List is empty.");
//     check(index >= 0 && index < list->count, "Index out of bounds.");

//     List *new_list = calloc(1, sizeof(List));
//     ListNode *temp = calloc(1, sizeof(ListNode));
//     check_mem(new_list);
//     check_mem(temp);
    
//     ListNode *current = list->first;
//     check_mem(current);

//     for (int i = 0; i < index; i++) {
//         current = current->next;
//     }

//     for (int i = index; i < list->count; i++) {
//         temp = current;
//         List_push(new_list, temp->value);
//         current = current->next;
//     }

//     current = list->first;

//     for (int i = 0; i < index; i++) {
//         current = current->next;
//     }

//     while (current) {
//         temp = current->next;
//         List_remove(list, current);
//         current = temp;
//     }

//     List_print(new_list);
//     List_print(list);

//     return new_list;

// error:
//     return NULL;
// }


// Efficient implementation w pointer manipulation
void *List_split(List *list, int index)
{
    // Basic checks
    check(list != NULL, "List is NULL.");
    check(list->count > 0, "List is empty.");
    check(index >= 0 && index < list->count, "Index out of bounds.");

    // If splitting at 0, the new list is everything, old list becomes empty
    if (index == 0) {
        List *new_list = list;      // Entire list goes to new_list
        list = List_create();       // Old list is now empty
        return new_list;
    }

    // If splitting at the end, new list is empty, old list remains the same
    if (index == list->count) {
        List *new_list = List_create();
        return new_list;
    }

    List *new_list = List_create();
    check_mem(new_list);

    // Traverse to the node at position `index`.
    ListNode *current = list->first;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }

    // `current` is the first node of the new list
    new_list->first = current;
    new_list->last  = list->last;

    // Adjust pointers on the old list
    list->last = current->prev;
    list->last->next = NULL;   // End the old list

    // Adjust pointers on the new list
    new_list->first->prev = NULL;

    // Update counts
    new_list->count = list->count - index;
    list->count = index; 

    return new_list;

error:
    return NULL;
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