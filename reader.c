#include "reader.h"

#define BUFFER_SIZE_INIT 2048

// Reads the entire contents of a file in a single read.
// Returns {0, NULL} on error.
struct String read_file(FILE *file) {
    fseek(file, 0, SEEK_SET);
    size_t buffer_size = BUFFER_SIZE_INIT;
    uint8_t *buffer = malloc(buffer_size);
    if (buffer == NULL)
        return (struct String){0, NULL};
    while (1) {
        size_t bytes_read = fread(buffer, 1, buffer_size, file);
        if (ferror(file))
            return (struct String){0, NULL};
        if (feof(file))
            return (struct String){bytes_read, buffer};
        buffer_size *= 2;
        buffer = realloc(buffer, buffer_size);
        if (buffer == NULL)
            return (struct String){0, NULL};
    }
}
