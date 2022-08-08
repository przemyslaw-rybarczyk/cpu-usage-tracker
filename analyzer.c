#include <stdlib.h>
#include <string.h>

#include "analyzer.h"

// Reads a number from str, starting at index i.
// Skips any spaces following the number.
// If the number is invalid, sets err to true.
static uint64_t read_number(struct String *str, size_t *i, bool *err) {
    uint64_t n = 0;
    for (; *i < str->len && str->data[*i] != ' ' && str->data[*i] != '\n'; (*i)++) {
        uint8_t c = str->data[*i];
        if ('0' <= c && c <= '9')
            n = 10 * n + (c - '0');
        else
            *err = true;
    }
    // Skip spaces
    for (; *i < str->len && str->data[*i] == ' '; (*i)++)
        ;
    return n;
}

// Parses the string str containing data from /proc/stat.
// Returns the number of CPU cores.
uint64_t get_num_cpu_cores(struct String *str) {
    uint64_t num_cores = 0;
    for (size_t i = 0; i < str->len; i++) {
        // Check if line starts with "cpu"
        if (i <= str->len - 3 && memcmp(str->data + i, "cpu", 3) == 0) {
            i += 3;
            // Get cpu number and values
            bool err = false;
            uint64_t core_i = read_number(str, &i, &err);
            if (!err && core_i >= num_cores)
                num_cores = core_i + 1;
        }
        // Skip rest of line
        for (; i < str->len && str->data[i] != '\n'; i++)
            ;
    }
    return num_cores;
}

// Parses the string str containing data from /proc/stat.
// Returns an arrays of CPU core time data for each of the num_cores cores.
struct CPUTime *parse_cpu_time(struct String *str, uint64_t num_cores) {
    struct CPUTime *cpu_time = malloc(num_cores * sizeof(struct CPUTime));
    if (cpu_time == NULL)
        return NULL;
    memset(cpu_time, 0, num_cores * sizeof(struct CPUTime));
    for (size_t i = 0; i < str->len; i++) {
        // Check if line starts with "cpu"
        if (i <= str->len - 3 && memcmp(str->data + i, "cpu", 3) == 0) {
            i += 3;
            // Get cpu number and values
            bool err = false;
            uint64_t core_i = read_number(str, &i, &err);
            if (core_i < num_cores) {
                cpu_time[core_i].user = read_number(str, &i, &err);
                cpu_time[core_i].nice = read_number(str, &i, &err);
                cpu_time[core_i].system = read_number(str, &i, &err);
                cpu_time[core_i].idle = read_number(str, &i, &err);
                cpu_time[core_i].iowait = read_number(str, &i, &err);
                cpu_time[core_i].irq = read_number(str, &i, &err);
                cpu_time[core_i].softirq = read_number(str, &i, &err);
                cpu_time[core_i].steal = read_number(str, &i, &err);
                cpu_time[core_i].guest = read_number(str, &i, &err);
                cpu_time[core_i].guest_nice = read_number(str, &i, &err);
                if (err) {
                    // Reset values to defaults
                    memset(&cpu_time[core_i], 0, sizeof(struct CPUTime));
                }
            }
        }
        // Skip rest of line
        for (; i < str->len && str->data[i] != '\n'; i++)
            ;
    }
    return cpu_time;
}

static uint64_t get_cpu_idle_usage(struct CPUTime *cpu_time) {
    return cpu_time->idle + cpu_time->iowait;
}

static uint64_t get_cpu_nonidle_usage(struct CPUTime *cpu_time) {
    return cpu_time->user + cpu_time->nice + cpu_time->system + cpu_time->irq + cpu_time->softirq + cpu_time->steal;
}

// Calculates the usage for each core based on the current and previous CPUTime values.
// Return an array of doubles of length num_cores.
double *get_cpu_usage(struct CPUTime *prev_cpu_time, struct CPUTime *cpu_time, uint64_t num_cores) {
    double *usage = malloc(num_cores * sizeof(double));
    if (usage == NULL)
        return NULL;
    for (uint64_t i = 0; i < num_cores; i++) {
        uint64_t prev_idle = get_cpu_idle_usage(&prev_cpu_time[i]);
        uint64_t idle = get_cpu_idle_usage(&cpu_time[i]);
        uint64_t prev_nonidle = get_cpu_nonidle_usage(&prev_cpu_time[i]);
        uint64_t nonidle = get_cpu_nonidle_usage(&cpu_time[i]);
        uint64_t prev_total = prev_idle + prev_nonidle;
        uint64_t total = idle + nonidle;
        usage[i] = 100.0 * (double)(nonidle - prev_nonidle) / (double)(total - prev_total);
    }
    return usage;
}
