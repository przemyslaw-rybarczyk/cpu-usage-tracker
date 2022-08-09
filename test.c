#include <assert.h>
#include <stdio.h>

#include "types.h"
#include "reader.h"
#include "analyzer.h"

static void test_read_file(void) {
    FILE *file = fopen("/proc/stat", "r");
    assert(file != NULL);
    struct String *str = read_file(file);
    assert(str != NULL);
    free(str);
}

static const char *test_proc_stat_string =
    "cpu 1 2 3 4 5 6 7 8 9 10\n"
    "cpu3 11 12 13 14 15 16 17 18 19 20 21 22\n"
    "cpu1 23 24 25 26 27 28 29 30 31 32\n";

static void test_get_num_cpu_cores(struct String *test_proc_stat) {
    uint64_t cpu_cores = get_num_cpu_cores(test_proc_stat);
    assert(cpu_cores == 4);
}

static void test_parse_cpu_time(struct String *test_proc_stat) {
    struct CPUTime *cpu_time = parse_cpu_time(test_proc_stat, 4);
    assert(cpu_time[0].user == 0);
    assert(cpu_time[0].nice == 0);
    assert(cpu_time[0].guest_nice == 0);
    assert(cpu_time[1].user == 23);
    assert(cpu_time[1].nice == 24);
    assert(cpu_time[1].guest_nice == 32);
    assert(cpu_time[2].user == 0);
    assert(cpu_time[2].nice == 0);
    assert(cpu_time[2].guest_nice == 0);
    assert(cpu_time[3].user == 11);
    assert(cpu_time[3].nice == 12);
    assert(cpu_time[3].guest_nice == 20);
    free(cpu_time);
}

int main() {
    struct String *test_proc_stat = malloc(sizeof(struct String) + strlen(test_proc_stat_string));
    test_proc_stat->len = strlen(test_proc_stat_string);
    memcpy(test_proc_stat->data, test_proc_stat_string, strlen(test_proc_stat_string));
    printf("Running test_read_file\n");
    test_read_file();
    printf("Running test_get_num_cpu_cores\n");
    test_get_num_cpu_cores(test_proc_stat);
    printf("Running test_parse_cpu_time\n");
    test_parse_cpu_time(test_proc_stat);
    printf("All tests passed\n");
}
