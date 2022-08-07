#pragma once

#include <inttypes.h>
#include <stdbool.h>

struct String {
    size_t len;
    uint8_t *data;
};
