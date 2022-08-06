#pragma once

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

struct String {
    size_t len;
    uint8_t *data;
};

struct String read_file(FILE *file);
