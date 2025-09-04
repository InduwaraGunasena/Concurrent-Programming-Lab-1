/**
 * rwlock.c - Reader-Writer Lock implementation of the concurrent linked list lab
 *
 * Usage:
 *   ./rwlock n m mMember mInsert mDelete num_threads [seed]
 *
 * Output:
 *   Prints CSV: case,rwlock,num_threads,rep,time_us,seed
 */
#define _XOPEN_SOURCE 700

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

pthread_rwlock_t rwlock;
pthread_barrier_t barrier;

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
    char type;
    int val;
} op_t;

typedef struct {
    int thread_id;
    int start;
    int end;
    op_t* ops;
} thread_arg_t;

static inline long long get_time_us() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

void* Thread_work(void* arg) {
    thread_arg_t* a = (thread_arg_t*)arg;

    pthread_barrier_wait(&barrier);

    for (int i = a->start; i < a->end; i++) {
        if (a->ops[i].type == 'M') {
            pthread_rwlock_rdlock(&rwlock);
            Member(a->ops[i].val, head);
            pthread_rwlock_unlock(&rwlock);
        } else if (a->ops[i].type == 'I') {
            pthread_rwlock_wrlock(&rwlock);
            Insert(a->ops[i].val, &head);
            pthread_rwlock_unlock(&rwlock);
        } else {
            pthread_rwlock_wrlock(&rwlock);
            Delete(a->ops[i].val, &head);
            pthread_rwlock_unlock(&rwlock);
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 9) {
        fprintf(stderr, "Usage: %s n m mMember mInsert mDelete num_threads seed case_id\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    double mMember = atof(argv[3]);
    double mInsert = atof(argv[4]);
    double mDelete = atof(argv[5]);
    int thread_count = atoi(argv[6]);
    unsigned int seed = (argc > 7) ? (unsigned int)atoi(argv[7]) : (unsigned int)time(NULL);
    srand(seed);
    int case_id = atoi(argv[8]);


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

    pthread_rwlock_init(&rwlock, NULL);
    pthread_barrier_init(&barrier, NULL, thread_count + 1);

    for (int run = 0; run < NUM_RUNS; run++) {
        head = NULL;
        int inserted = 0;
        while (inserted < n) {
            int val = rand() % MAX_KEY;
            if (Insert(val, &head)) inserted++;
        }

        pthread_t* threads = malloc(thread_count * sizeof(pthread_t));
        thread_arg_t* args = malloc(thread_count * sizeof(thread_arg_t));

        int ops_per_thread = m / thread_count;
        for (int t = 0; t < thread_count; t++) {
            args[t].thread_id = t;
            args[t].start = t * ops_per_thread;
            args[t].end = (t == thread_count - 1) ? m : (t + 1) * ops_per_thread;
            args[t].ops = ops;
            pthread_create(&threads[t], NULL, Thread_work, &args[t]);
        }

        long long start = get_time_us();
        pthread_barrier_wait(&barrier);

        for (int t = 0; t < thread_count; t++) {
            pthread_join(threads[t], NULL);
        }
        long long end = get_time_us();

        printf("%d,rwlock,%d,%d,%lld,%u\n", case_id, thread_count, run, (end - start), seed);

        Free_list(head);
        free(threads);
        free(args);
    }

    pthread_rwlock_destroy(&rwlock);
    pthread_barrier_destroy(&barrier);
    free(ops);

    return 0;
}

