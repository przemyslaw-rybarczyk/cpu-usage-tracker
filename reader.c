#include "reader.h"

#define BUFFER_SIZE_INIT 2048

// Reads the entire contents of a file in a single read.
// Returns NULL on error.
struct String *read_file(FILE *file) {
    fseek(file, 0, SEEK_SET);
    size_t buffer_size = BUFFER_SIZE_INIT;
    struct String *str = malloc(sizeof(struct String) + buffer_size);
    if (str == NULL)
        return NULL;
    while (1) {
        size_t bytes_read = fread(&str->data, 1, buffer_size, file);
        if (ferror(file))
            return NULL;
        if (feof(file)) {
            str->len = bytes_read;
            return str;
        }
        buffer_size *= 2;
        str = realloc(str, sizeof(struct String) + buffer_size);
        if (str == NULL)
            return NULL;
    }
}
