#include <stdio.h>
#include <time.h>

#include "types.h"
#include "logger.h"
#include "buffer.h"

struct Buffer *log_buffer;

struct LogMsg {
    time_t time;
    const char *text;
};

void log_msg(const char *text) {
    struct LogMsg *msg = malloc(sizeof(struct LogMsg));
    msg->time = time(NULL);
    msg->text = text;
    buffer_push(log_buffer, (void *)msg);
}

static char time_str[9];

void *logger_main(void *arg) {
    FILE *log_file = fopen("log", "w");
    if (log_file == 0)
        return NULL;
    while (1) {
        struct LogMsg *msg = (struct LogMsg *)buffer_pop(log_buffer);
        strftime(time_str, sizeof(time_str), "%T", localtime(&msg->time));
        fprintf(log_file, "%s - %s\n", time_str, msg->text);
        fflush(log_file);
        free(msg);
    }
}
