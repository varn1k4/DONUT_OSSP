#include <stdio.h>
#include <stdlib.h>

int main()
{
    FILE *file;
    char line[256];

    printf("============================================\n");
    printf("             CPU MONITORING\n");
    printf("============================================\n");

    file = fopen("/proc/cpuinfo", "r");

    if (file == NULL)
    {
        printf("Unable to open /proc/cpuinfo\n");
        return 1;
    }

    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == 'm' && line[1] == 'o' && line[2] == 'd' &&
            line[3] == 'e' && line[4] == 'l' && line[5] == ' ')
        {
            printf("%s", line);
        }

        if (line[0] == 'c' && line[1] == 'p' && line[2] == 'u' &&
            line[3] == ' ' && line[4] == 'M')
        {
            printf("%s", line);
        }

        if (line[0] == 'c' && line[1] == 'p' && line[2] == 'u' &&
            line[3] == ' ' && line[4] == 'c')
        {
            printf("%s", line);
        }
    }

    fclose(file);

    printf("============================================\n");

    return 0;
}
