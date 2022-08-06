#include <stdio.h>

#include "reader.h"

int main() {
    FILE *file = fopen("/proc/stat", "r");
    if (file == NULL)
        return 1;
    struct String str = read_file(file);
    if (str.data == NULL)
        return 1;
    for (size_t i = 0; i < str.len; i++)
        putchar(str.data[i]);
}
