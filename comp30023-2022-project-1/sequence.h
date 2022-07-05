/* This header file is created to allow us to keep track of any sequences of 
'connected' processes in order to detect a deadlock in this sequence.
The code for the linked list is taken from Alistair Moffat's textbook, "Programming, 
problem solving, and abstraction with C", 2013. He taught the subject COMP10002 at
The University of Melbourne, and given students permission to use his code. */ 

/* This is one process recorded in a sequence list */ 
typedef struct process_in_sequence process_in_sequence_t; 

struct process_in_sequence {
    int processID;
    int lockID; 
    int waitID; 
    process_in_sequence_t *next; 
};

typedef struct {
    process_in_sequence_t *head;
    process_in_sequence_t *foot; 
} sequence_list_t;

/* make a new, empty list */
sequence_list_t *make_empty_sequence_list(void);

/* returns 1 if it is an empty list */
int is_empty_sequence_list(sequence_list_t *sequence_list);

void free_sequence_list(sequence_list_t *sequence_list); 

/* insert process to the sequence list */
sequence_list_t *add_process_to_sequence(sequence_list_t *sequence_list, 
int processID, int lockID, int waitID);

/* get the process that is at the head of the list */
process_in_sequence_t *get_sequence_head(sequence_list_t *sequence_list); 

/* delete all the processes in sequence list, refreshing it */
sequence_list_t *erase_sequence(sequence_list_t *sequence_list); 

/* check to see if there is a deadlock inside the sequence's list of processes.
return 1 if there is a deadlock, 0 otherwise */
int check_sequence_deadlock(sequence_list_t *sequence_list); 

/* returns process which needs to be eliminated from sequence in order to avoid deadlock */
process_in_sequence_t *find_process_cause_deadlock(sequence_list_t *sequence_list); 