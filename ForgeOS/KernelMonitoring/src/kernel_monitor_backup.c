#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

#define MAX_PROCESSES 1024

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

    printf("%-8s %-30s %-8s %-12s\n",
           "PID", "PROCESS", "STATE", "MEMORY(KB)");

    printf("------------------------------------------------------------\n");

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

                    printf("%-8d %-30s %-8c %-12lu\n",
                           pid, process_name, state, memory);

                    count++;
                }

                fclose(stat_file);
            }
        }
    }

    closedir(dir);

    printf("------------------------------------------------------------\n");
    printf("Total processes displayed: %d\n", count);
}

int main()
{
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
    printf("             MONITORING COMPLETED\n");
    printf("============================================================\n");

    return 0;
}

