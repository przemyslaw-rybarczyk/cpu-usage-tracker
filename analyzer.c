#include <stdlib.h>
#include <string.h>

#include "analyzer.h"

// Reads a number from str, starting at index i.
// Skips any spaces following the number.
// If the number is invalid, sets err to true.
static uint64_t read_number(struct String str, size_t *i, bool *err) {
    uint64_t n = 0;
    for (; *i < str.len && str.data[*i] != ' ' && str.data[*i] != '\n'; (*i)++) {
        uint8_t c = str.data[*i];
        if ('0' <= c && c <= '9')
            n = 10 * n + (c - '0');
        else
            *err = true;
    }
    // Skip spaces
    for (; *i < str.len && str.data[*i] == ' '; (*i)++)
        ;
    return n;
}

// Parses the string str containing data from /proc/stat.
// Returns an arrays of CPU core usage data for each of the num_cores cores.
struct Usage *parse_cpu_usage(struct String str, uint64_t num_cores) {
    struct Usage *usage = malloc(num_cores * sizeof(struct Usage));
    if (usage == NULL)
        return NULL;
    memset(usage, 0, num_cores * sizeof(struct Usage));
    for (size_t i = 0; i < str.len; i++) {
        // Check if line starts with "cpu"
        if (i <= str.len - 3 && memcmp(str.data + i, "cpu", 3) == 0) {
            i += 3;
            // Get cpu number and values
            bool err = false;
            uint64_t core_i = read_number(str, &i, &err);
            if (core_i < num_cores) {
                usage[core_i].user = read_number(str, &i, &err);
                usage[core_i].nice = read_number(str, &i, &err);
                usage[core_i].system = read_number(str, &i, &err);
                usage[core_i].idle = read_number(str, &i, &err);
                usage[core_i].iowait = read_number(str, &i, &err);
                usage[core_i].irq = read_number(str, &i, &err);
                usage[core_i].softirq = read_number(str, &i, &err);
                usage[core_i].steal = read_number(str, &i, &err);
                usage[core_i].guest = read_number(str, &i, &err);
                usage[core_i].guest_nice = read_number(str, &i, &err);
                if (err) {
                    // Reset values to defaults
                    memset(&usage[core_i], 0, sizeof(struct Usage));
                }
            }
        }
        // Skip rest of line
        for (; i < str.len && str.data[i] != '\n'; i++)
            ;
    }
    return usage;
}
