#include <stdio.h>
#include <pthread.h>

int shared_counter = 0;

pthread_mutex_t lock;

void *worker(void *arg)
{
    int thread_id = *(int *)arg;

    pthread_mutex_lock(&lock);

    printf("Thread %d entered critical section.\n", thread_id);

    shared_counter++;

    printf("Thread %d updated shared counter to %d.\n",
           thread_id, shared_counter);

    pthread_mutex_unlock(&lock);

    return NULL;
}

int main()
{
    pthread_t thread1, thread2;

    int id1 = 1;
    int id2 = 2;

    printf("========================================\n");
    printf("       CO-6 THREADS AND SYNCHRONIZATION\n");
    printf("========================================\n");

    pthread_mutex_init(&lock, NULL);

    printf("Creating two threads...\n");

    pthread_create(&thread1, NULL, worker, &id1);
    pthread_create(&thread2, NULL, worker, &id2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("\nFinal shared counter value: %d\n", shared_counter);

    pthread_mutex_destroy(&lock);

    printf("Threads completed successfully.\n");
    printf("CO-6 demonstration completed.\n");

    return 0;
}
