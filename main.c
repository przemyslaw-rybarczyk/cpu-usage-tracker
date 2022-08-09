#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>

#include "reader.h"
#include "analyzer.h"
#include "printer.h"
#include "logger.h"
#include "buffer.h"

#define RAW_DATA_BUFFER_CAPACITY 8
#define CORE_USAGE_BUFFER_CAPACITY 8
#define LOG_BUFFER_CAPACITY 64

static uint64_t num_cores;

static struct Buffer *raw_data_buffer;
static struct Buffer *core_usage_buffer;

static pthread_t reader_thread;
static pthread_t analyzer_thread;
static pthread_t printer_thread;
static pthread_t watchdog_thread;
static pthread_t logger_thread;

static atomic_bool reader_alive = false;
static atomic_bool analyzer_alive = false;
static atomic_bool printer_alive = false;

static void *reader_main(void *arg) {
    FILE *file = (FILE *)arg;
    while (1) {
        reader_alive = true;
        log_msg("Reader: reading /proc/stat");
        struct String *str = read_file(file);
        if (str == NULL) {
            log_msg("Reader: read failed");
            return NULL;
        }
        log_msg("Reader: sending file contents");
        buffer_push(raw_data_buffer, (void *)str);
        log_msg("Reader: sleeping");
        sleep(1);
    }
}

static struct CPUTime *analyzer_get_cpu_time(void) {
    log_msg("Analyzer: getting file contents");
    struct String *str = (struct String *)buffer_pop(raw_data_buffer);
    log_msg("Analyzer: parsing /proc/stat");
    struct CPUTime *cpu_time = parse_cpu_time(str, num_cores);
    free(str);
    if (cpu_time == NULL)
        log_msg("Analyzer: failed to parse /proc/stat");
    return cpu_time;
}

static void *analyzer_main(void *arg) {
    struct CPUTime *prev_cpu_time = analyzer_get_cpu_time();
    if (prev_cpu_time == NULL)
        return NULL;
    while (1) {
        analyzer_alive = true;
        struct CPUTime *cpu_time = analyzer_get_cpu_time();
        if (cpu_time == NULL)
            return NULL;
        double *usage = get_cpu_usage(prev_cpu_time, cpu_time, num_cores);
        if (usage == NULL) {
            log_msg("Analyzer: get_cpu_usage() failed");
            return NULL;
        }
        log_msg("Analyzer: sending CPU usage");
        buffer_push(core_usage_buffer, (void *)usage);
        free(prev_cpu_time);
        prev_cpu_time = cpu_time;
    }
}

static void *printer_main(void *arg) {
    while (1) {
        printer_alive = true;
        log_msg("Printer: getting usage");
        double *usage = (double *)buffer_pop(core_usage_buffer);
        log_msg("Printer: displaying usage");
        print_cpu_usage(usage, num_cores);
        free(usage);
    }
}

static void *watchdog_main(void *arg) {
    while (1) {
        sleep(2);
        log_msg("Watchdog: running check");
        bool kill_process = false;
        if (!reader_alive) {
            log_msg("Watchdog: no message from reader");
            kill_process = true;
        }
        if (!analyzer_alive) {
            log_msg("Watchdog: no message from analyzer");
            kill_process = true;
        }
        if (!printer_alive) {
            log_msg("Watchdog: no message from printer");
            kill_process = true;
        }
        if (kill_process)
            exit(1);
        reader_alive = false;
        analyzer_alive = false;
        printer_alive = false;
    }
}

int main() {
    raw_data_buffer = buffer_new(RAW_DATA_BUFFER_CAPACITY);
    core_usage_buffer = buffer_new(CORE_USAGE_BUFFER_CAPACITY);
    log_buffer = buffer_new(LOG_BUFFER_CAPACITY);
    FILE *file = fopen("/proc/stat", "r");
    if (file == NULL) {
        fprintf(stderr, "Error: failed to open /proc/stat\n");
        return errno;
    }
    struct String *str = read_file(file);
    if (str == NULL)
        return 1;
    num_cores = get_num_cpu_cores(str);
    free(str);
    int reader_create_err = pthread_create(&reader_thread, NULL, reader_main, (void *)file);
    int analyzer_create_err = pthread_create(&analyzer_thread, NULL, analyzer_main, NULL);
    int printer_create_err = pthread_create(&printer_thread, NULL, printer_main, NULL);
    int watchdog_create_err = pthread_create(&watchdog_thread, NULL, watchdog_main, NULL);
    int logger_create_err = pthread_create(&logger_thread, NULL, logger_main, NULL);
    if (reader_create_err || analyzer_create_err || printer_create_err || watchdog_create_err || logger_create_err) {
        fprintf(stderr, "Error: failed to create thread\n");
        return 1;
    }
    log_msg("Main thread: Program initialization finished");
    pthread_exit(NULL);
}
