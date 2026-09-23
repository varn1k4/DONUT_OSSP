#include <stdio.h>

int main()
{
    FILE *file;
    char line[256];

    printf("============================================\n");
    printf("           MEMORY MONITORING\n");
    printf("============================================\n");

    file = fopen("/proc/meminfo", "r");

    if (file == NULL)
    {
        printf("Unable to open /proc/meminfo\n");
        return 1;
    }

    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == 'M' &&
            (line[3] == 'T' || line[3] == 'F' || line[3] == 'A'))
        {
            printf("%s", line);
        }
    }

    fclose(file);

    printf("============================================\n");

    return 0;
}
