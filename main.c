#include <stdio.h>

#include "reader.h"
#include "analyzer.h"

static float *read_usage(FILE *file, uint64_t num_cores) {
    struct String str = read_file(file);
    if (str.data == NULL)
        return NULL;
    for (size_t i = 0; i < str.len; i++)
        putchar(str.data[i]);
    return parse_cpu_usage(str, num_cores);
}

int main() {
    uint64_t num_cores = 4;
    FILE *file = fopen("/proc/stat", "r");
    if (file == NULL)
        return 1;
    struct Usage *prev_usage = read_usage(file, num_cores);
    if (prev_usage == NULL)
        return 1;
    sleep(1);
    struct Usage *usage = read_usage(file, num_cores);
    if (usage == NULL)
        return 1;
    double *usage_percent = get_cpu_usage_percent(prev_usage, usage, num_cores);
    if (usage_percent == NULL)
        return 1;
    printf("\nUsage per core:\n");
    for (uint64_t i = 0; i < num_cores; i++)
        printf("%f%%\n", usage_percent[i]);
}
