#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>

/* ==================== CO-3 SIGNAL HANDLER ==================== */

void signal_handler(int sig)
{
    printf("\nSIGINT signal received!\n");
    printf("Signal handling completed.\n");
}

/* ==================== CO-6 THREAD DATA ==================== */

int shared_counter = 0;
pthread_mutex_t lock;

/* ==================== CO-6 WORKER ==================== */

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

/* ==================== CO-1 PROCESS MANAGEMENT ==================== */

void co1_process_management()
{
    pid_t pid;

    printf("\n========================================\n");
    printf("       CO-1 PROCESS MANAGEMENT\n");
    printf("========================================\n");

    printf("Parent Process PID : %d\n", getpid());
    printf("Creating child process using fork()...\n");

    pid = fork();

    if (pid < 0)
    {
        printf("Process creation failed.\n");
        return;
    }

    if (pid == 0)
    {
        printf("\n--- Child Process ---\n");
        printf("Child PID  : %d\n", getpid());
        printf("Parent PID : %d\n", getppid());

        printf("Child process is executing...\n");
        printf("Child process terminated.\n");

        exit(0);
    }
    else
    {
        printf("\n--- Parent Process ---\n");
        printf("Parent PID : %d\n", getpid());
        printf("Child PID  : %d\n", pid);

        wait(NULL);

        printf("Parent detected child termination.\n");
        printf("Parent process completed.\n");
    }
}

/* ==================== CO-3 SIGNALS AND PIPE ==================== */

void co3_signals_pipe()
{
    int pipefd[2];
    pid_t pid;

    char message[] = "Message from child process";
    char buffer[100];

    printf("\n========================================\n");
    printf("       CO-3 SIGNALS AND PIPE\n");
    printf("========================================\n");

    signal(SIGINT, signal_handler);

    printf("Signal handler registered for SIGINT.\n");

    if (pipe(pipefd) == -1)
    {
        printf("Pipe creation failed.\n");
        return;
    }

    printf("Pipe created successfully.\n");

    pid = fork();

    if (pid < 0)
    {
        printf("Process creation failed.\n");
        return;
    }

    if (pid == 0)
    {
        close(pipefd[0]);

        write(pipefd[1], message, strlen(message) + 1);

        printf("\n--- Child Process ---\n");
        printf("Child PID : %d\n", getpid());
        printf("Message sent through pipe.\n");

        close(pipefd[1]);
        exit(0);
    }
    else
    {
        close(pipefd[1]);

        read(pipefd[0], buffer, sizeof(buffer));

        printf("\n--- Parent Process ---\n");
        printf("Parent PID : %d\n", getpid());
        printf("Received message: %s\n", buffer);

        close(pipefd[0]);

        wait(NULL);

        printf("Child process completed.\n");
        printf("CO-3 demonstration completed.\n");
    }
}

/* ==================== CO-4 MEMORY MANAGEMENT ==================== */

void co4_memory_management()
{
    int *memory;
    void *mapped_memory;

    printf("\n========================================\n");
    printf("       CO-4 MEMORY MANAGEMENT\n");
    printf("========================================\n");

    printf("\n--- Dynamic Memory Allocation ---\n");

    memory = (int *)malloc(5 * sizeof(int));

    if (memory == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    for (int i = 0; i < 5; i++)
    {
        memory[i] = (i + 1) * 10;
    }

    printf("Memory allocated using malloc().\n");
    printf("Stored values: ");

    for (int i = 0; i < 5; i++)
    {
        printf("%d ", memory[i]);
    }

    printf("\n");

    free(memory);

    printf("Memory released using free().\n");

    printf("\n--- Memory Mapping ---\n");

    mapped_memory = mmap(NULL, 4096,
                         PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS,
                         -1, 0);

    if (mapped_memory == MAP_FAILED)
    {
        printf("Memory mapping failed.\n");
        return;
    }

    strcpy((char *)mapped_memory,
           "Memory mapped successfully.");

    printf("%s\n", (char *)mapped_memory);

    munmap(mapped_memory, 4096);

    printf("Mapped memory released using munmap().\n");
    printf("CO-4 demonstration completed.\n");
}

/* ==================== CO-6 THREADS ==================== */

void co6_threads_synchronization()
{
    pthread_t thread1, thread2;

    int id1 = 1;
    int id2 = 2;

    shared_counter = 0;

    printf("\n========================================\n");
    printf("       CO-6 THREADS AND SYNCHRONIZATION\n");
    printf("========================================\n");

    pthread_mutex_init(&lock, NULL);

    printf("Creating two threads...\n");

    pthread_create(&thread1, NULL, worker, &id1);
    pthread_create(&thread2, NULL, worker, &id2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("\nFinal shared counter value: %d\n",
           shared_counter);

    pthread_mutex_destroy(&lock);

    printf("Threads completed successfully.\n");
    printf("CO-6 demonstration completed.\n");
}

/* ==================== MAIN MENU ==================== */

int main()
{
    int choice;

    while (1)
    {
        printf("\n========================================\n");
        printf("       OS CONCEPTS DEMONSTRATION\n");
        printf("========================================\n");
        printf("1. CO-1 Process Management\n");
        printf("2. CO-3 Signals and Pipe\n");
        printf("3. CO-4 Memory Management\n");
        printf("4. CO-6 Threads and Synchronization\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");

        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                co1_process_management();
                break;

            case 2:
                co3_signals_pipe();
                break;

            case 3:
                co4_memory_management();
                break;

            case 4:
                co6_threads_synchronization();
                break;

            case 5:
                printf("Exiting program.\n");
                return 0;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
