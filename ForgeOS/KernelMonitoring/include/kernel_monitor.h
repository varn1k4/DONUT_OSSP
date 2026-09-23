#ifndef KERNEL_MONITOR_H
#define KERNEL_MONITOR_H

#define MAX_PROCESSES 1024
#define CPU_THRESHOLD 5.0
#define MEMORY_THRESHOLD 50000

typedef struct {
    int pid;
    char name[256];
    char state;
    unsigned long memory;
    double cpu_usage;
    char status[30];
} ProcessInfo;

void read_cpu_info(void);
void read_memory_info(void);
void monitor_processes(void);

#endif
