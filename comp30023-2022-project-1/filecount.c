#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "filecount.h"

/* make a new, empty list */
record_list_t *make_empty_record_list(void) {
    record_list_t *record_list; 
    record_list = (record_list_t*)malloc(sizeof(*record_list)); 
    assert(record_list != NULL);
    record_list->head = record_list->foot = NULL; 
    return record_list; 
}

/* returns 1 if it is an empty list */
int is_empty_record_list(record_list_t *record_list) {
    assert(record_list != NULL);
    return record_list->head == NULL; 
}

void free_record_list(record_list_t *record_list) {
    record_t *curr, *prev;
    assert(record_list != NULL);
    curr = record_list->head; 
    while (curr) {
        prev = curr;
        curr = curr->next; 
        free(prev); 
    }
    free(record_list); 
}

/* insert record to the list */
record_list_t *insert_record(record_list_t *record_list, int waitID, int count) {
    record_t *new; 
    new = (record_t*)malloc(sizeof(*new)); 
    assert(record_list != NULL && new != NULL); 
    new->waitID = waitID; 
    new->count = count; 
    new->next = NULL; 
    if (record_list->foot == NULL) {
        /* this is the first insertion into the list */
        record_list->head = record_list->foot = new; 
    } else {
        record_list->foot->next = new; 
        record_list->foot = new;
    }
    return record_list;
}

/* get the record that is at the head of the list */
record_t *get_head(record_list_t *record_list) {
    assert(record_list != NULL && record_list->head != NULL);
    return record_list->head; 
}