#include <stdio.h>

#include "reader.h"
#include "analyzer.h"

static struct CPUTime *read_cpu_time(FILE *file, uint64_t num_cores) {
    struct String str = read_file(file);
    if (str.data == NULL)
        return NULL;
    for (size_t i = 0; i < str.len; i++)
        putchar(str.data[i]);
    return parse_cpu_time(str, num_cores);
}

int main() {
    uint64_t num_cores = 4;
    FILE *file = fopen("/proc/stat", "r");
    if (file == NULL)
        return 1;
    struct CPUTime *prev_cpu_time = read_cpu_time(file, num_cores);
    if (prev_cpu_time == NULL)
        return 1;
    sleep(1);
    struct CPUTime *cpu_time = read_cpu_time(file, num_cores);
    if (cpu_time == NULL)
        return 1;
    double *usage = get_cpu_usage(prev_cpu_time, cpu_time, num_cores);
    if (usage == NULL)
        return 1;
    printf("\nUsage per core:\n");
    for (uint64_t i = 0; i < num_cores; i++)
        printf("%f%%\n", usage[i]);
}
