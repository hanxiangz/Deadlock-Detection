#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "sequence.h"

/* make a new, empty list */
sequence_list_t *make_empty_sequence_list(void) {
    sequence_list_t *sequence_list; 
    sequence_list = (sequence_list_t*)malloc(sizeof(*sequence_list)); 
    assert(sequence_list != NULL);
    sequence_list->head = sequence_list->foot = NULL; 
    return sequence_list; 
}

/* returns 1 if it is an empty list */
int is_empty_sequence_list(sequence_list_t *sequence_list) {
    assert(sequence_list != NULL);
    return sequence_list->head == NULL; 
}

void free_sequence_list(sequence_list_t *sequence_list) {
    process_in_sequence_t *curr, *prev;
    assert(sequence_list != NULL);
    curr = sequence_list->head; 
    while (curr) {
        prev = curr;
        curr = curr->next; 
        free(prev); 
    }
    free(sequence_list);
}

/* insert process to the sequence list */
sequence_list_t *add_process_to_sequence(sequence_list_t *sequence_list, 
int processID, int lockID, int waitID) {
    process_in_sequence_t *new; 
    new = (process_in_sequence_t*)malloc(sizeof(*new)); 
    assert(sequence_list != NULL && new != NULL); 
    new->waitID = waitID; 
    new->lockID = lockID;
    new->processID = processID;  
    new->next = NULL; 
    if (sequence_list->foot == NULL) {
        /* this is the first insertion into the list */
        sequence_list->head = sequence_list->foot = new; 
    } else {
        sequence_list->foot->next = new; 
        sequence_list->foot = new;
    }
    return sequence_list;
}

/* get the process that is at the head of the list */
process_in_sequence_t *get_sequence_head(sequence_list_t *sequence_list) {
    assert(sequence_list != NULL && sequence_list->head != NULL);
    return sequence_list->head; 
}

/* delete all the processes in sequence list, refreshing it */
sequence_list_t *erase_sequence(sequence_list_t *sequence_list) {
    sequence_list->head = NULL; 
    sequence_list->foot = NULL; 
    return sequence_list;
}

/* check to see if there is a deadlock inside the sequence's list of processes.
return 1 if there is a deadlock, 0 otherwise */
int check_sequence_deadlock(sequence_list_t *sequence_list) {
    int deadlock = 0; 
    if (is_empty_sequence_list(sequence_list)) {
        return 0; 
    } else if (sequence_list->head == sequence_list->foot) { // only 1 process in list
        return 0; 
    } else { // multiple processes in list, which means chance of deadlock
        process_in_sequence_t *temp_process = sequence_list->head; 
        /* create temp_process to traverse through the list */
        while (temp_process != NULL) {
            /* if the waitID of the last process occurs as lockID in this list, then deadlock */
            if (temp_process->lockID == sequence_list->foot->waitID) {
                deadlock = 1; 
            }
            if (temp_process->next == sequence_list->foot) {
                break; 
            }
            temp_process = temp_process->next; 
        }
    }

    return deadlock;
    
}

/* returns process which needs to be eliminated from sequence in order to avoid deadlock.
It is the one with the smallest process ID */
process_in_sequence_t *find_process_cause_deadlock(sequence_list_t *sequence_list) {
    process_in_sequence_t *process; 
    if (sequence_list->head->next == sequence_list->foot) { // there are 2 elements in the sequence
        process =  sequence_list->head; 
    } else {
        process = sequence_list->head->next;
    }
    return process; 
}