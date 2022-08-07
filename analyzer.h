#pragma once

#include "types.h"

struct Usage {
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

struct Usage *parse_cpu_usage(struct String str, uint64_t num_cores);
double *get_cpu_usage_percent(struct Usage *prev_usage, struct Usage *usage, uint64_t num_cores);
