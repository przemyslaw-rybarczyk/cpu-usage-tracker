#pragma once

#include "types.h"

struct CPUTime {
    uint64_t user;
    uint64_t nice;
    uint64_t system;
    uint64_t idle;
    uint64_t iowait;
    uint64_t irq;
    uint64_t softirq;
    uint64_t steal;
    uint64_t guest;
    uint64_t guest_nice;
};

struct CPUTime *parse_cpu_time(struct String str, uint64_t num_cores);
double *get_cpu_usage(struct CPUTime *prev_cpu_time, struct CPUTime *cpu_time, uint64_t num_cores);
