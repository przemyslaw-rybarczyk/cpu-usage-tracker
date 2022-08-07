#include <stdio.h>

#include "reader.h"
#include "analyzer.h"

int main() {
    FILE *file = fopen("/proc/stat", "r");
    if (file == NULL)
        return 1;
    struct String str = read_file(file);
    if (str.data == NULL)
        return 1;
    for (size_t i = 0; i < str.len; i++)
        putchar(str.data[i]);
    uint64_t num_cores = 4;
    struct Usage *usage = parse_cpu_usage(str, num_cores);
    printf("\nUsage per core:\n");
    for (uint64_t i = 0; i < num_cores; i++)
        printf("%lu\n", usage[i].user);
}
