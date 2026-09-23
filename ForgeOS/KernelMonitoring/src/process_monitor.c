#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>

int is_number(const char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}

void monitor_processes()
{
    DIR *dir;
    struct dirent *entry;

    dir = opendir("/proc");

    if (dir == NULL)
    {
        perror("Unable to open /proc");
        return;
    }

    printf("\n");
    printf("============================================================\n");
    printf("                 PROCESS MONITORING\n");
    printf("============================================================\n");
    printf("%-10s %-25s %-10s\n", "PID", "PROCESS", "STATE");
    printf("------------------------------------------------------------\n");

    while ((entry = readdir(dir)) != NULL)
    {
        if (is_number(entry->d_name))
        {
            char path[512];
            char name[100];
            char state;

snprintf(path, sizeof(path),
                     "/proc/%s/stat", entry->d_name);

            FILE *file = fopen(path, "r");

            if (file != NULL)
            {
                int pid;
                char process_name[100];

                if (fscanf(file, "%d %99s %c",
                           &pid, process_name, &state) == 3)
                {
                    printf("%-10d %-25s %-10c\n",
                           pid, process_name, state);
                }

                fclose(file);
            }
        }
    }

    closedir(dir);

    printf("============================================================\n");
}

int main()
{
    printf("============================================\n");
    printf("        KERNEL MONITORING SYSTEM\n");
    printf("============================================\n");

    printf("Reading process information from /proc...\n");

    monitor_processes();

    printf("\nMonitoring completed.\n");

    return 0;
}
