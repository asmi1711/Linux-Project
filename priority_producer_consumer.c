#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 10
#define NUM_PRODUCERS 3
#define NUM_CONSUMERS 3
#define NUM_ITEMS 20

typedef struct {
    int buffer[BUFFER_SIZE];
    int in;
    int out;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} Queue;

Queue queue;

void queue_init(Queue *q) {
    q->in = q->out = q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_full, NULL);
    pthread_cond_init(&q->not_empty, NULL);
}

void enqueue(Queue *q, int item) {
    q->buffer[q->in] = item;
    q->in = (q->in + 1) % BUFFER_SIZE;
    q->count++;
}

int dequeue(Queue *q) {
    int item = q->buffer[q->out];
    q->out = (q->out + 1) % BUFFER_SIZE;
    q->count--;
    return item;
}

// Adds a high-priority item at the front of the queue
void enqueue_priority(Queue *q, int item) {
    q->out = (q->out - 1 + BUFFER_SIZE) % BUFFER_SIZE;
    q->buffer[q->out] = item;
    q->count++;
}

void *producer(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < NUM_ITEMS; i++) {
        pthread_mutex_lock(&queue.mutex);
        while (queue.count == BUFFER_SIZE)
            pthread_cond_wait(&queue.not_full, &queue.mutex);

        int item = id * 100 + i;
        enqueue(&queue, item);
        printf("Producer %d produced item %d\n", id, item);

        pthread_cond_signal(&queue.not_empty);
        pthread_mutex_unlock(&queue.mutex);

        usleep(rand() % 300000);
    }
    return NULL;
}

void *priority_producer(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < NUM_ITEMS / 2; i++) {
        pthread_mutex_lock(&queue.mutex);
        while (queue.count == BUFFER_SIZE)
            pthread_cond_wait(&queue.not_full, &queue.mutex);

        int item = 9990 + i;
        enqueue_priority(&queue, item);
        printf("Priority Producer %d added item %d (front of queue)\n", id, item);

        pthread_cond_signal(&queue.not_empty);
        pthread_mutex_unlock(&queue.mutex);

        usleep(rand() % 500000);
    }
    return NULL;
}

void *consumer(void *arg) {
    int id = *(int *)arg;
    for (;;) {
        pthread_mutex_lock(&queue.mutex);
        while (queue.count == 0)
            pthread_cond_wait(&queue.not_empty, &queue.mutex);

        int item = dequeue(&queue);
        printf("Consumer %d consumed item %d\n", id, item);

        pthread_cond_signal(&queue.not_full);
        pthread_mutex_unlock(&queue.mutex);

        usleep(rand() % 400000);
    }
    return NULL;
}

int main() {
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];
    pthread_t priorityProd;
    int ids[NUM_PRODUCERS + NUM_CONSUMERS + 1];

    queue_init(&queue);

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        ids[i] = i;
        pthread_create(&consumers[i], NULL, consumer, &ids[i]);
    }

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        ids[i] = i;
        pthread_create(&producers[i], NULL, producer, &ids[i]);
    }

    int priority_id = 99;
    pthread_create(&priorityProd, NULL, priority_producer, &priority_id);

    for (int i = 0; i < NUM_PRODUCERS; i++)
        pthread_join(producers[i], NULL);
    pthread_join(priorityProd, NULL);

    sleep(2);
    for (int i = 0; i < NUM_CONSUMERS; i++)
        pthread_cancel(consumers[i]);

    pthread_mutex_destroy(&queue.mutex);
    pthread_cond_destroy(&queue.not_full);
    pthread_cond_destroy(&queue.not_empty);

    printf("All threads completed.\n");
    return 0;
}
