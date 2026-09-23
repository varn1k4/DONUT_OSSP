#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>

#define MAX_PROCESSES 1024
#define CPU_THRESHOLD 5.0
#define MEMORY_THRESHOLD 50000

int is_number(const char *str)
{
    if (*str == '\0')
        return 0;

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit((unsigned char)str[i]))
            return 0;
    }

    return 1;
}

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

    unsigned long utime = 0;
    unsigned long stime = 0;

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

void show_cpu_info()
{
    FILE *file;
    char line[256];

    printf("\n");
    printf("============================================================\n");
    printf("                    CPU INFORMATION\n");
    printf("============================================================\n");

    file = fopen("/proc/cpuinfo", "r");

    if (file == NULL)
    {
        perror("Unable to open /proc/cpuinfo");
        return;
    }

    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "model name", 10) == 0)
        {
            char *value = strchr(line, ':');

            if (value != NULL)
                printf("CPU Model       : %s", value + 2);

            break;
        }
    }

    rewind(file);

    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "cpu cores", 9) == 0)
        {
            char *value = strchr(line, ':');

            if (value != NULL)
                printf("CPU Cores       : %s", value + 2);

            break;
        }
    }

    rewind(file);

    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "cpu MHz", 7) == 0)
        {
            char *value = strchr(line, ':');

            if (value != NULL)
                printf("CPU Frequency   : %s", value + 2);

            break;
        }
    }

    fclose(file);
}

void show_memory_info()
{
    FILE *file;
    char line[256];

    printf("\n");
    printf("============================================================\n");
    printf("                  MEMORY INFORMATION\n");
    printf("============================================================\n");

    file = fopen("/proc/meminfo", "r");

    if (file == NULL)
    {
        perror("Unable to open /proc/meminfo");
        return;
    }

    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "MemTotal:", 9) == 0 ||
            strncmp(line, "MemFree:", 8) == 0 ||
            strncmp(line, "MemAvailable:", 13) == 0)
        {
            printf("%s", line);
        }
    }

    fclose(file);
}

void show_processes()
{
    DIR *dir;
    struct dirent *entry;
    int count = 0;

    printf("\n");
    printf("============================================================\n");
    printf("                    PROCESS INFORMATION\n");
    printf("============================================================\n");

    printf("%-8s %-30s %-8s %-12s %-10s %-15s\n",
           "PID", "PROCESS", "STATE", "MEMORY(KB)",
           "CPU(%)", "STATUS");

    printf("--------------------------------------------------------------------------------\n");

    dir = opendir("/proc");

    if (dir == NULL)
    {
        perror("Unable to open /proc");
        return;
    }

    while ((entry = readdir(dir)) != NULL && count < MAX_PROCESSES)
    {
        if (is_number(entry->d_name))
        {
            char stat_path[512];
            char status_path[512];

            int pid;
            char process_name[100];
            char state;
            unsigned long memory = 0;

            snprintf(stat_path, sizeof(stat_path),
                     "/proc/%s/stat", entry->d_name);

            snprintf(status_path, sizeof(status_path),
                     "/proc/%s/status", entry->d_name);

            FILE *stat_file = fopen(stat_path, "r");

            if (stat_file != NULL)
            {
                if (fscanf(stat_file, "%d %99s %c",
                           &pid, process_name, &state) == 3)
                {
                    FILE *status_file = fopen(status_path, "r");

                    if (status_file != NULL)
                    {
                        char line[256];

                        while (fgets(line, sizeof(line), status_file))
                        {
                            if (strncmp(line, "VmRSS:", 6) == 0)
                            {
                                sscanf(line, "VmRSS: %lu", &memory);
                                break;
                            }
                        }

                        fclose(status_file);
                    }

                    unsigned long cpu_before = get_total_cpu_time();
                    unsigned long process_before =
                        get_process_cpu_time(pid);

                    usleep(100000);

                    unsigned long cpu_after = get_total_cpu_time();
                    unsigned long process_after =
                        get_process_cpu_time(pid);

                    unsigned long total_difference =
                        cpu_after - cpu_before;

                    unsigned long process_difference =
                        process_after - process_before;

                    double cpu_usage = 0.0;

                    if (total_difference > 0)
                    {
                        cpu_usage =
                            ((double)process_difference /
                             (double)total_difference) * 100.0;
                    }

                    char status[20];

                    if (cpu_usage >= CPU_THRESHOLD &&
                        memory >= MEMORY_THRESHOLD)
                    {
                        strcpy(status, "HIGH CPU+MEM");
                    }
                    else if (cpu_usage >= CPU_THRESHOLD)
                    {
                        strcpy(status, "HIGH CPU");
                    }
                    else if (memory >= MEMORY_THRESHOLD)
                    {
                        strcpy(status, "HIGH MEMORY");
                    }
                    else
                    {
                        strcpy(status, "Normal");
                    }

                    printf("%-8d %-30s %-8c %-12lu %-10.2f %-15s\n",
                           pid,
                           process_name,
                           state,
                           memory,
                           cpu_usage,
                           status);

                    count++;
                }

                fclose(stat_file);
            }
        }
    }

    closedir(dir);

    printf("--------------------------------------------------------------------------------\n");
    printf("Total processes displayed: %d\n", count);
}

int main()
{
    while (1)
    {
        printf("\033[H\033[J");

        printf("\n");
        printf("============================================================\n");
        printf("              KERNEL MONITORING SYSTEM\n");
        printf("============================================================\n");

        printf("\nReading information directly from Linux /proc...\n");

        show_cpu_info();
        show_memory_info();
        show_processes();

        printf("\n");
        printf("============================================================\n");
        printf("       MONITORING WILL REFRESH EVERY FEW SECONDS\n");
        printf("       Press Ctrl+C to exit\n");
        printf("============================================================\n");

        sleep(3);
    }

    return 0;
}
