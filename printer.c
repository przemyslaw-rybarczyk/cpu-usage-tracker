#include <stdio.h>

#include "printer.h"

#define BAR_WIDTH 60

const char *partial_segments[8] = {" ", "▏", "▎", "▍", "▌", "▋", "▊", "▉"};

void print_cpu_usage(double *usage, uint64_t num_cores) {
    for (uint64_t i = 0; i < num_cores; i++) {
        printf("%2"PRIu64" %6.2f%% [", i, usage[i]);
        uint32_t full_segments = (uint32_t)(usage[i] / 100.0 * BAR_WIDTH);
        for (uint32_t j = 0; j < full_segments; j++)
            printf("█");
        uint32_t partial_segment_i = (uint32_t)(usage[i] / 100.0 * BAR_WIDTH * 8) % 8;
        printf("%s", partial_segments[partial_segment_i]);
        for (uint32_t j = 0; j < BAR_WIDTH - full_segments - 1; j++)
            printf(" ");
        printf("]\n");
    }
    printf("\n");
}
