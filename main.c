#include <assert.h>
#include <stdio.h>

#include "reader.h"
#include "analyzer.h"
#include "printer.h"
#include "buffer.h"

static struct CPUTime *read_cpu_time(FILE *file, uint64_t num_cores) {
    struct String *str = read_file(file);
    if (str == NULL)
        return NULL;
    return parse_cpu_time(str, num_cores);
}

int main() {
    struct Buffer *buffer = buffer_new(8);
    buffer_push(buffer, (void *)buffer);
    void *e = buffer_pop(buffer);
    assert(e == (void *)buffer);
    buffer_free(buffer);
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
    print_cpu_usage(usage, num_cores);
}
