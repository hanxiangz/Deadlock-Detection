#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h> 

#include "filecount.h"
#include "sequence.h"

typedef struct {
    int process_id;
    int lock_id;
    int wait_id;
    int checked; // checks off this process as 'released' 
} Process;

/* print the number of processes and files in the system */
void print_num_process_file(Process processes[], int count); 

/* check to see if number is inside array, going through specified number of iterations.
return 1 if there is, 0 if not */
int num_in_array(int array[], int n_iterations, int num);

/* calculate the total execution time of a given list of processes.
'count' parameter is the number of processes in that given list */
int exec_time(Process processes[], int count); 

/* append wait_id to the record_list, together with its frequence of occurence */
void count_waitID(record_list_t *record_list, int wait_id);

/* detects any deadlocks in the list of processes read from file.
returns a list of processes causing the deadlocks */
void detect_deadlocks(Process processes[], int num_processes, 
sequence_list_t *sequence, sequence_list_t *processes_causing_deadlock); 

/* returns 1 if all processes have been checked/finished, 0 otherwise */
int all_processes_finished(Process processes[], int num_processes); 

/* sort (ascending) a list of processes according to process ID, printing process ID's */
void print_sorted_processes(sequence_list_t *processes_causing_deadlock, int sorted_processIDs[]); 

/* check that the process has unique file ids in the given list */
int check_process_unique(process_in_sequence_t *process, sequence_list_t *list); 

/* Add a process to a list */
void add_process_to_array(process_in_sequence_t *process, sequence_list_t *list);

/* Get processes that can be released per unit of time, without deadlocks */
void no_deadlocks(Process processes[], int process_count, sequence_list_t *processes_per_unit_time[], 
    int *index);

/* print all the process in the list */
void print_list_processes(sequence_list_t *list); 

int main(int argc, char* argv[]) { 

    /* Identify where input file is in command line, alongside optional parameters */
    int file_index;
    int parameter_e = 0, parameter_f = 0, parameter_c = 0; // 1 if the parameter exists, 0 if not
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            parameter_f = 1; 
            continue;
        } else if (strcmp(argv[i], "-e") == 0) {
            parameter_e = 1;  
        } else if (strcmp(argv[i], "-c") == 0) {
            parameter_c = 1; 
        } else {
            file_index = i;
            break;
        }
    }
    
    /* open file */
    FILE* fp = fopen(argv[file_index], "r");
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    } 

    /* Read into memory */
    size_t processes_count = 0, processes_size = 2; // count keeps track of number of process read 
    Process* processes = (Process*)malloc(sizeof(Process) * processes_size);
    if (processes == NULL) {
        fprintf(stderr, "Malloc failure\n");
        exit(EXIT_FAILURE);
    }

    int process_id, lock_id, wait_id;

    while (fscanf(fp, "%d %d %d", &process_id, &lock_id, &wait_id) == 3) {
        if (processes_count == processes_size) {
            processes_size *= 2;
            processes = realloc(processes, sizeof(Process) * processes_size);
            if (processes == NULL) {
                fprintf(stderr, "Realloc failure\n");
                exit(EXIT_FAILURE);
            }
        }
        processes[processes_count].process_id = process_id;
        processes[processes_count].lock_id = lock_id;
        processes[processes_count++].wait_id = wait_id; 
    }

    if (parameter_f == 1 && parameter_c == 0 && parameter_e == 0) { 
        print_num_process_file(processes, processes_count); // task 1
        /* Create a sequence list to keep track of any processes that are 'linked' */
        sequence_list_t *sequence = make_empty_sequence_list(); 
        sequence_list_t *processes_causing_deadlock = make_empty_sequence_list();
        detect_deadlocks(processes, processes_count,
            sequence, processes_causing_deadlock); 
        if (processes_causing_deadlock->head == NULL) { // task 3
            printf("No deadlocks\n");
        } else {
            printf("Deadlock detected\n"); 
            int num_processes_causing_deadlock = 0; 
            process_in_sequence_t *temp_process = processes_causing_deadlock->head; 
            while (temp_process != NULL) {
                num_processes_causing_deadlock++; 
                temp_process = temp_process->next; 
            }
            int sorted_processIDs[num_processes_causing_deadlock]; 
            print_sorted_processes(processes_causing_deadlock, sorted_processIDs); // task 4 & 5
            free(temp_process); 
        }
        free_sequence_list(sequence); 
        sequence = NULL;
        free_sequence_list(processes_causing_deadlock); 
        processes_causing_deadlock = NULL; 
    } else if (parameter_f == 1 && parameter_c == 0 && parameter_e == 1) { // task 2
        print_num_process_file(processes, processes_count);
        printf("Execution time %d", exec_time(processes, processes_count)); 
    } else if (parameter_f == 1 && parameter_c == 1 && parameter_e == 0) { // task 6
        /* Each index in this 2D list stores the processes with unique file IDs, 
        which will be released per unit of time. */
        sequence_list_t *processes_per_unit_time[processes_count]; 
        for (int i = 0; i < processes_count; i++) {
            processes_per_unit_time[i] = make_empty_sequence_list(); 
        }
        int index = 0;  
        no_deadlocks(processes, processes_count, processes_per_unit_time, &index); 
        for (int j = 0; j < index; j++) {
            printf("%d ", index); 
            print_list_processes(processes_per_unit_time[j]); 
        }

    }

    free(processes);
    fclose(fp); 

    return 0; 
}

void print_num_process_file(Process processes[], int count) {
    int max_num_files = count * 2;
    int num_unique = 0; 
    int unique_files[max_num_files]; 
    
    /* read through the lock_id in all lines in system to sort unique files */
    for (int i = 0; i < count; i++) {
        /* check to see if the lockid is already in the array */
        if (num_in_array(unique_files, num_unique, processes[i].lock_id)) {
            continue; 
        } else {
            unique_files[num_unique++] = processes[i].lock_id; 
        }
    } 

    /* read through the wait_id in all lines in system to sort unique files */
    for (int i = 0; i < count; i++) {
        /* check to see if the wait_id is already in the array */
        if (num_in_array(unique_files, num_unique, processes[i].wait_id)) {
            continue; 
        } else {
            unique_files[num_unique++] = processes[i].wait_id; 
        }
    } 

    printf("Processes %d\n", count); 
    printf("Files %d\n", num_unique); 
}

int num_in_array(int array[], int n_iterations, int num) {
    int found_num = 0; 
    for (int i = 0; i < n_iterations; i++) {
        if (array[i] == num) {
            found_num = 1;
            break; 
        }
    }
    return found_num; 
}

int exec_time(Process processes[], int count) {
    /* this list keeps track of the counts of each requested 'wait' file */
    record_list_t *record_list; 
    record_list = make_empty_record_list(); 
    /* count each wait_id into the linked list */
    for (int i = 0; i < count; i++) {
        if (is_empty_record_list(record_list)) { // first record inserted 
            record_list = insert_record(record_list, processes[i].wait_id, 1);  
        } else {
            count_waitID(record_list, processes[i].wait_id); 
        }
    }

    /* Now we have a list showing the frequency of each requested file id. */
    int highest_freq = 0;  
    record_t *temp_record = record_list->head;
    /* create a temp_record to traverse through the list */
    while (temp_record != NULL) {
        if (temp_record->count > highest_freq) {
            highest_freq = temp_record->count; 
        }
        temp_record = temp_record->next; 
    }

    free_record_list(record_list); 
    record_list = NULL;
    
    return highest_freq + 1; // it takes 1 unit of time to read all lock id's
    
}

void count_waitID(record_list_t *record_list, int wait_id) {
    record_t *temp_record = record_list->head; 
    int waitID_found = 0; 
    /* create a temp_record to traverse through the list */
    while (temp_record != NULL) {
        if (temp_record->waitID == wait_id) {
            temp_record->count++; 
            waitID_found = 1; 
        }
        temp_record = temp_record->next; 
    }

    if (waitID_found == 0) {
        record_list = insert_record(record_list, wait_id, 1);
    }

    free(temp_record);

} 

/* detects any deadlocks in the list of processes read from file.
returns a list of processes causing the deadlocks */
void detect_deadlocks(Process processes[], int num_processes, 
sequence_list_t *sequence, sequence_list_t *processes_causing_deadlock) {

    for (int i = 0; i < num_processes; i++) {
        if (processes[i].checked == 1) { // process already finished
            if (i == num_processes - 1) { // we are at the final element in processes array
                erase_sequence(sequence); 
                if (all_processes_finished(processes, num_processes)) { // all processes are done
                    erase_sequence(sequence);
                    break; 
                } else {
                    detect_deadlocks(processes, num_processes, sequence, processes_causing_deadlock);
                }
            } else {
                continue; 
            }
        } else { // process not yet finished
            if (is_empty_sequence_list(sequence)) {
                processes[i].checked = 1; 
                add_process_to_sequence(sequence, processes[i].process_id, 
                processes[i].lock_id, processes[i].wait_id);
                detect_deadlocks(processes, num_processes, sequence, processes_causing_deadlock);
            } else {
                if (processes[i].lock_id == sequence->foot->waitID) {
                    processes[i].checked = 1; 
                    add_process_to_sequence(sequence, processes[i].process_id, 
                        processes[i].lock_id, processes[i].wait_id);
                    /* check to see if there is a deadlock found in the sequence */
                    if (check_sequence_deadlock(sequence)) {
                        /* the smallest ID of the process causing deadlock */
                        int process_id = find_process_cause_deadlock(sequence)->processID; 
                        int lock_id = find_process_cause_deadlock(sequence)->lockID; 
                        int wait_id = find_process_cause_deadlock(sequence)->waitID; 
                        add_process_to_sequence(processes_causing_deadlock, process_id, lock_id, wait_id); 
                    } else {
                        detect_deadlocks(processes, num_processes, sequence, processes_causing_deadlock);
                    }
                } else {
                    if (i == num_processes - 1) { // we are at the end of the proccesses array
                        erase_sequence(sequence); 
                        if (all_processes_finished(processes, num_processes)) { // all processes are done
                            erase_sequence(sequence);
                            break; 
                        } else {
                            detect_deadlocks(processes, num_processes, sequence, 
                                processes_causing_deadlock);
                        }
                    } else {
                        continue; 
                    }
                }
            }
        }
    }

    return;
}


/* returns 1 if all processes have been checked/finished, 0 otherwise */
int all_processes_finished(Process processes[], int num_processes) {
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].checked == 0) {
            return 0; 
        }
    }
    return 1; 
}

/* sort (ascending) a list of processes according to process ID, printing process ID's */
void print_sorted_processes(sequence_list_t *processes_causing_deadlock, int sorted_processIDs[]) {
    process_in_sequence_t *temp_process = processes_causing_deadlock->head; 
    int num_processes = 0;
    while (temp_process != NULL) {
        sorted_processIDs[num_processes++] = temp_process->processID;  
        temp_process = temp_process->next; 
    }

    /* now sort the array */
    for (int i = 1; i < num_processes; i++) {
        for (int j = i - 1; j >= 0 && sorted_processIDs[j+1] < sorted_processIDs[j]; j--) {
            int temp = sorted_processIDs[j]; 
            sorted_processIDs[j] = sorted_processIDs[j+1]; 
            sorted_processIDs[j+1] = temp; 
        }
    }

    /* print out processes that need to be resolved */
    printf("Terminate "); 
    for (int k = 0; k < num_processes; k++) {
        printf("%d ", sorted_processIDs[k]); 
    }

    free(temp_process); 

    return; 
}

/* check that the process has unique file ids in the given list */
int check_process_unique(process_in_sequence_t *process, sequence_list_t *list) {
    int unique_or_not = 1; 
    if (list == NULL) {
        return unique_or_not;
    }
    process_in_sequence_t *temp = list->head; 
    while (list->foot != NULL) {
        if (temp->lockID == process->lockID || temp->waitID == process->waitID) {
            unique_or_not = 0; 
        }
        temp = temp->next; 
    }
    return unique_or_not; 
}

/* Add a process to a list */
void add_process_to_array(process_in_sequence_t *process, sequence_list_t *list) {
    process_in_sequence_t *new = process; 
    list->foot->next = new; 
    list->foot = new; 
}

/* Get processes that can be released per unit of time, without deadlocks */
void no_deadlocks(Process processes[], int process_count, sequence_list_t *processes_per_unit_time[], 
    int *index) {

    for (int i = 0; i < process_count; i++) {
        process_in_sequence_t *process = (process_in_sequence_t*)malloc(sizeof(*process));
        process->processID = processes[i].process_id; 
        process->lockID = processes[i].lock_id; 
        process->waitID = processes[i].wait_id; 
        process->next = NULL; 
        if (processes[i].checked) {
            continue; 
        } 

        if (check_process_unique(process, processes_per_unit_time[*index])) {
            processes[i].checked = 1;  
            add_process_to_array(process, processes_per_unit_time[*index]); 
            continue;
        } else {
            if (i == process_count) {
                if (all_processes_finished(processes, process_count)) {
                    break;
                } else {
                    *index+=1;  
                    no_deadlocks(processes, process_count, processes_per_unit_time, index);
                }
            } else {
                continue; 
            }
        }
    }
    return; 
}

/* print all the process in the list */
void print_list_processes(sequence_list_t *list) {
    process_in_sequence_t *temp_process = list->head;
    while (temp_process != NULL) {
        printf("%d %d,%d\n", temp_process->processID, temp_process->lockID, temp_process->waitID); 
        temp_process = temp_process->next; 
    }
    return; 
}