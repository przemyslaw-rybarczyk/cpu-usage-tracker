#include <assert.h>
#include <pthread.h>
#include <stdio.h>

#include "reader.h"
#include "analyzer.h"
#include "printer.h"
#include "buffer.h"

#define RAW_DATA_BUFFER_CAPACITY 8
#define CORE_USAGE_BUFFER_CAPACITY 8

static uint64_t num_cores = 4;
static struct Buffer *raw_data_buffer;
static struct Buffer *core_usage_buffer;
static pthread_t reader_thread;
static pthread_t analyzer_thread;
static pthread_t printer_thread;

static void *reader_main(void *arg) {
    FILE *file = fopen("/proc/stat", "r");
    if (file == NULL)
        return NULL;
    while (1) {
        struct String *str = read_file(file);
        if (str == NULL)
            return NULL;
        buffer_push(raw_data_buffer, (void *)str);
        sleep(1);
    }
}

static void *analyzer_main(void *arg) {
    struct String *str = (struct String *)buffer_pop(raw_data_buffer);
    struct CPUTime *prev_cpu_time = parse_cpu_time(str, num_cores);
    if (prev_cpu_time == NULL)
        return NULL;
    while (1) {
        struct String *str = (struct String *)buffer_pop(raw_data_buffer);
        struct CPUTime *cpu_time = parse_cpu_time(str, num_cores);
        if (cpu_time == NULL)
            return NULL;
        double *usage = get_cpu_usage(prev_cpu_time, cpu_time, num_cores);
        if (usage == NULL)
            return NULL;
        buffer_push(core_usage_buffer, (void *)usage);
        prev_cpu_time = cpu_time;
    }
}

static void *printer_main(void *arg) {
    while (1) {
        double *usage = (double *)buffer_pop(core_usage_buffer);
        print_cpu_usage(usage, num_cores);
    }
}

int main() {
    raw_data_buffer = buffer_new(RAW_DATA_BUFFER_CAPACITY);
    core_usage_buffer = buffer_new(CORE_USAGE_BUFFER_CAPACITY);
    int reader_create_err = pthread_create(&reader_thread, NULL, reader_main, NULL);
    if (reader_create_err != 0) {
        fprintf(stderr, "Error: failed to create thread\n");
        return reader_create_err;
    }
    int analyzer_create_err = pthread_create(&analyzer_thread, NULL, analyzer_main, NULL);
    if (analyzer_create_err != 0) {
        fprintf(stderr, "Error: failed to create thread\n");
        return analyzer_create_err;
    }
    int printer_create_err = pthread_create(&printer_thread, NULL, printer_main, NULL);
    if (printer_create_err != 0) {
        fprintf(stderr, "Error: failed to create thread\n");
        return printer_create_err;
    }
    pthread_exit(NULL);
}
