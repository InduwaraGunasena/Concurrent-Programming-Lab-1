/**
 * serial.c - Serial implementation of the concurrent linked list lab
 *
 * Usage:
 *   ./serial n m mMember mInsert mDelete [seed]
 *
 * Arguments:
 *   n        - number of initial unique elements
 *   m        - total number of operations
 *   mMember  - fraction of member operations (e.g., 0.99)
 *   mInsert  - fraction of insert operations
 *   mDelete  - fraction of delete operations
 *   seed     - (optional) random seed
 *
 * Output:
 *   Prints one CSV line: case,implementation,threads,rep,time_us,seed
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <assert.h>

#define MAX_KEY 65535
#define NUM_RUNS 10

typedef struct list_node_s {
    int data;
    struct list_node_s* next;
} list_node_t;

list_node_t* head = NULL;

int Member(int value, list_node_t* head) {
    list_node_t* curr = head;
    while (curr != NULL && curr->data < value) {
        curr = curr->next;
    }
    return (curr != NULL && curr->data == value);
}

int Insert(int value, list_node_t** head_pp) {
    list_node_t* curr = *head_pp;
    list_node_t* pred = NULL;
    list_node_t* temp;

    while (curr != NULL && curr->data < value) {
        pred = curr;
        curr = curr->next;
    }

    if (curr == NULL || curr->data > value) {
        temp = malloc(sizeof(list_node_t));
        temp->data = value;
        temp->next = curr;
        if (pred == NULL)
            *head_pp = temp;
        else
            pred->next = temp;
        return 1;
    }
    return 0;
}

int Delete(int value, list_node_t** head_pp) {
    list_node_t* curr = *head_pp;
    list_node_t* pred = NULL;

    while (curr != NULL && curr->data < value) {
        pred = curr;
        curr = curr->next;
    }

    if (curr != NULL && curr->data == value) {
        if (pred == NULL)
            *head_pp = curr->next;
        else
            pred->next = curr->next;
        free(curr);
        return 1;
    }
    return 0;
}

void Free_list(list_node_t* head) {
    list_node_t* curr;
    while (head != NULL) {
        curr = head;
        head = head->next;
        free(curr);
    }
}

typedef struct {
    char type;   // 'M', 'I', 'D'
    int val;
} op_t;

static inline long long get_time_us() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

int main(int argc, char* argv[]) {
    if (argc < 7) {
        fprintf(stderr, "Usage: %s n m mMember mInsert mDelete seed case_id\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    double mMember = atof(argv[3]);
    double mInsert = atof(argv[4]);
    double mDelete = atof(argv[5]);
    unsigned int seed = (argc > 6) ? (unsigned int)atoi(argv[6]) : (unsigned int)time(NULL);
    srand(seed);
    int case_id = atoi(argv[7]);


    int mMemberOps = (int)(m * mMember);
    int mInsertOps = (int)(m * mInsert);
    int mDeleteOps = m - mMemberOps - mInsertOps;

    op_t* ops = malloc(m * sizeof(op_t));
    assert(ops);

    for (int i = 0; i < mMemberOps; i++) { ops[i].type = 'M'; ops[i].val = rand() % MAX_KEY; }
    for (int i = 0; i < mInsertOps; i++) { ops[mMemberOps + i].type = 'I'; ops[mMemberOps + i].val = rand() % MAX_KEY; }
    for (int i = 0; i < mDeleteOps; i++) { ops[mMemberOps + mInsertOps + i].type = 'D'; ops[mMemberOps + mInsertOps + i].val = rand() % MAX_KEY; }

    // Shuffle
    for (int i = 0; i < m; i++) {
        int j = rand() % m;
        op_t tmp = ops[i];
        ops[i] = ops[j];
        ops[j] = tmp;
    }

    for (int run = 0; run < NUM_RUNS; run++) {
        head = NULL;
        int inserted = 0;
        while (inserted < n) {
            int val = rand() % MAX_KEY;
            if (Insert(val, &head)) inserted++;
        }

        long long start = get_time_us();
        for (int i = 0; i < m; i++) {
            switch (ops[i].type) {
                case 'M': Member(ops[i].val, head); break;
                case 'I': Insert(ops[i].val, &head); break;
                case 'D': Delete(ops[i].val, &head); break;
            }
        }
        long long end = get_time_us();

        printf("%d,serial,1,%d,%lld,%u\n", case_id, run, (end - start), seed);

        Free_list(head);
        head = NULL;
    }

    free(ops);
    return 0;
}
