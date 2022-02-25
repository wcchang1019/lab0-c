#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    // struct list_head *list = malloc(sizeof(struct list_head));
    element_t *e = malloc(sizeof(element_t));
    if (!e) {
        return NULL;
    }
    e->value = NULL;
    INIT_LIST_HEAD(&e->list);
    return &e->list;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l) {
        return;
    }
    struct list_head *li;
    struct list_head *lx;
    list_for_each_safe (li, lx, l) {
        element_t *e = list_entry(li, element_t, list);
        free(e->value);
        free(e);
    }

    /* Free the start element */
    element_t *e = list_entry(l, element_t, list);
    free(e->value);
    free(e);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    char *new_s = malloc(strlen(s) + 1);
    if (!new_s) {
        return false;
    }
    strncpy(new_s, s, strlen(s) + 1);
    element_t *new_e = malloc(sizeof(element_t));
    if (!new_e) {
        free(new_s);
        return false;
    }
    new_e->value = new_s;
    INIT_LIST_HEAD(&new_e->list);
    list_add(&new_e->list, head);
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    char *new_s = malloc(strlen(s) + 1);
    if (!new_s) {
        return false;
    }
    strncpy(new_s, s, strlen(s) + 1);
    element_t *new_e = malloc(sizeof(element_t));
    if (!new_e) {
        free(new_s);
        return false;
    }
    new_e->value = new_s;
    INIT_LIST_HEAD(&new_e->list);
    list_add_tail(&new_e->list, head);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    // remove_head must run in constant time
    // So can not use q_size() to check list is empty or not
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *e = list_entry(head->next, element_t, list);
    if (sp) {
        strncpy(sp, e->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(head->next);
    return e;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    // remove_tail must run in constant time
    // So can not use q_size() to check list is empty or not
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *e = list_entry(head->prev, element_t, list);
    if (sp) {
        strncpy(sp, e->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(head->prev);
    return e;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int len = 0;
    struct list_head *li;
    list_for_each (li, head)
        len++;
    return len;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (q_size(head) == 0) {
        return false;
    }
    int idx = q_size(head) / 2;
    for (int i = 0; i < idx; i++) {
        head = head->next;
    }
    element_t *e = list_entry(head->next, element_t, list);
    list_del(head->next);
    // list_del is not responsible for freeing memory
    q_release_element(e);
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (q_size(head) == 0) {
        return false;
    }
    char *value = NULL;
    struct list_head *li;
    struct list_head *lx;
    struct list_head *dup_start_head = NULL;
    int count = 0;
    list_for_each_safe (li, lx, head) {
        element_t *e = list_entry(li, element_t, list);
        if ((e->value && !value) || (strcmp(e->value, value) != 0)) {
            value = e->value;
            if (count) {
                // Delete the first duplicate element
                element_t *dup_start_e =
                    list_entry(dup_start_head, element_t, list);
                list_del(dup_start_head);
                q_release_element(dup_start_e);
            }
            dup_start_head = li;
            count = 0;
        } else {
            // Delete other duplicate element
            list_del(li);
            q_release_element(e);
            count++;
        }
    }
    if (count) {
        // Delete the first duplicate element
        element_t *dup_start_e = list_entry(dup_start_head, element_t, list);
        list_del(dup_start_head);
        q_release_element(dup_start_e);
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (q_size(head) <= 1) {
        return;
    }
    struct list_head *start = head;
    struct list_head *slow = head->next;
    struct list_head *fast = head->next->next;
    while (!(slow == start) && !(fast == start)) {
        struct list_head *to_slow = slow->prev->next;
        struct list_head *to_fast = fast->next->prev;
        struct list_head *fast_next = fast->next;
        struct list_head *slow_prev = slow->prev;
        slow->prev->next = slow->next;
        fast->next->prev = fast->prev;
        fast->next = to_slow;
        slow->prev = to_fast;
        slow->next = fast_next;
        fast->prev = slow_prev;
        slow = slow->next;
        fast = slow->next;
    }
    return;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (q_size(head) <= 1) {
        return;
    }
    struct list_head *start = head;
    do {
        struct list_head *tmp = head->next;
        head->next = head->prev;
        head->prev = tmp;
        head = tmp;
    } while (head != start);
}

struct list_head *q_merge(struct list_head *l1, struct list_head *l2)
{
    struct list_head *new_head = NULL;
    struct list_head *new_head_result = NULL;
    while (l1 && l2) {
        element_t *e1 = list_entry(l1, element_t, list);
        element_t *e2 = list_entry(l2, element_t, list);
        if (strcmp(e1->value, e2->value) > 0) {
            if (!new_head) {
                new_head = l2;
                new_head_result = new_head;
            } else {
                new_head->next = l2;
                new_head = new_head->next;
            }
            l2 = l2->next;
        } else {
            if (!new_head) {
                new_head = l1;
                new_head_result = new_head;
            } else {
                new_head->next = l1;
                new_head = new_head->next;
            }
            l1 = l1->next;
        }
    }
    if (l1) {
        new_head->next = l1;
    } else if (l2) {
        new_head->next = l2;
    }
    return new_head_result;
}

struct list_head *merge_sort(struct list_head *head)
{
    if (!head || !head->next) {
        return head;
    }
    struct list_head *fast = head->next;
    struct list_head *slow = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;
    head = merge_sort(head);
    fast = merge_sort(fast);
    return q_merge(head, fast);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    if (q_size(head) <= 1) {
        return;
    }
    // Turn circular doubly-linked list to doubly-linked list
    head->prev->next = NULL;
    struct list_head *sorted_list = merge_sort(head->next);
    head->next = sorted_list;
    // Assign every element with new prev pointer
    struct list_head *prev = head;
    while (sorted_list) {
        sorted_list->prev = prev;
        prev = sorted_list;
        sorted_list = sorted_list->next;
    }
    // Turn to circular doubly-linked list
    head->prev = prev;
    prev->next = head;
}

void q_shuffle(struct list_head *head)
{
    int size = q_size(head);
    if (size <= 1) {
        return;
    }
    struct list_head *li;
    int i = size - 1;
    for (li = head->prev; li != head; li = li->prev) {
        int j = rand() % (i + 1);
        if (i != j) {
            struct list_head *lj = head->next;
            while (j) {
                lj = lj->next;
                j--;
            }
            element_t *e1 = list_entry(li, element_t, list);
            element_t *e2 = list_entry(lj, element_t, list);
            char *tmp = e1->value;
            e1->value = e2->value;
            e2->value = tmp;
        }
        i--;
    }
}