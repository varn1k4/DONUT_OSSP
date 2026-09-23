#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

unsigned long get_total_cpu_time()
{
    FILE *file;
    char line[256];

    unsigned long user, nice, system, idle;
    unsigned long iowait, irq, softirq, steal;

    file = fopen("/proc/stat", "r");

    if (file == NULL)
        return 0;

    fgets(line, sizeof(line), file);

    sscanf(line, "cpu %lu %lu %lu %lu %lu %lu %lu %lu",
           &user, &nice, &system, &idle,
           &iowait, &irq, &softirq, &steal);

    fclose(file);

    return user + nice + system + idle +
           iowait + irq + softirq + steal;
}

unsigned long get_process_cpu_time(int pid)
{
    char path[256];
    char line[1024];

    unsigned long utime;
    unsigned long stime;

    snprintf(path, sizeof(path), "/proc/%d/stat", pid);

    FILE *file = fopen(path, "r");

    if (file == NULL)
        return 0;

    if (fgets(line, sizeof(line), file) == NULL)
    {
        fclose(file);
        return 0;
    }

    fclose(file);

    char *end = strrchr(line, ')');

    if (end == NULL)
        return 0;

    end++;

    sscanf(end,
           " %*c %*d %*d %*d %*d %*d %*d %*d %*d %*d %lu %lu",
           &utime, &stime);

    return utime + stime;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <PID>\n", argv[0]);
        return 1;
    }

    int pid = atoi(argv[1]);

    unsigned long cpu1;
    unsigned long cpu2;

    unsigned long process1;
    unsigned long process2;

    printf("\n");
    printf("============================================================\n");
    printf("                 PROCESS CPU MONITOR\n");
    printf("============================================================\n");

    printf("Monitoring PID: %d\n", pid);

    cpu1 = get_total_cpu_time();
    process1 = get_process_cpu_time(pid);

    sleep(1);

    cpu2 = get_total_cpu_time();
    process2 = get_process_cpu_time(pid);

    unsigned long total_difference = cpu2 - cpu1;
    unsigned long process_difference = process2 - process1;

    double cpu_usage = 0.0;

    if (total_difference > 0)
    {
        cpu_usage = ((double)process_difference /
                     (double)total_difference) * 100.0;
    }

    printf("CPU Usage       : %.2f%%\n", cpu_usage);

    printf("============================================================\n");

    return 0;
}
