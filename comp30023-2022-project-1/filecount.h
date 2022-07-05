/* This header file is used to keep track of how many times a requested file appears.
The code for the linked list is taken from Alistair Moffat's textbook, "Programming, 
problem solving, and abstraction with C", 2013. He taught the subject COMP10002 at
The University of Melbourne, and given students permission to use his code. */ 

/* 1 record refers to a requested file and its corresponding frequence of occurence */
typedef struct record record_t; 

struct record {
    int waitID; 
    int count; 
    record_t *next; 
};

typedef struct {
    record_t *head;
    record_t *foot; 
} record_list_t; 

/* make a new, empty list */
record_list_t *make_empty_record_list(void);

/* returns 1 if it is an empty list */
int is_empty_record_list(record_list_t *record_list); 

void free_record_list(record_list_t *record_list); 

/* insert record to the list */
record_list_t *insert_record(record_list_t *record_list, int waitID, int count); 

/* get the record that is at the head of the list */
record_t *get_head(record_list_t *record_list); 
