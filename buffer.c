#include <pthread.h>

#include "buffer.h"

struct Buffer {
    size_t cap;
    size_t len;
    size_t start;
    size_t end;
    pthread_mutex_t mutex;
    pthread_cond_t full_cond;
    pthread_cond_t empty_cond;
    void *data[];
};

struct Buffer *buffer_new(size_t capacity) {
    struct Buffer *buffer = malloc(sizeof(struct Buffer) + capacity * sizeof(void *));
    if (buffer == NULL)
        return NULL;
    buffer->cap = capacity;
    buffer->len = 0;
    buffer->start = 0;
    buffer->end = 0;
    if (pthread_mutex_init(&buffer->mutex, NULL))
        return NULL;
    if (pthread_cond_init(&buffer->full_cond, NULL))
        return NULL;
    if (pthread_cond_init(&buffer->empty_cond, NULL))
        return NULL;
    return buffer;
}

void buffer_push(struct Buffer *buffer, void *elem) {
    if (pthread_mutex_lock(&buffer->mutex))
        pthread_exit(NULL);
    while (buffer->len == buffer->cap) {
        if (pthread_cond_wait(&buffer->full_cond, &buffer->mutex))
            pthread_exit(NULL);
    }
    buffer->data[buffer->end] = elem;
    buffer->end = (buffer->end + 1) % buffer->cap;
    buffer->len++;
    if (pthread_cond_signal(&buffer->empty_cond)) {
        pthread_mutex_unlock(&buffer->mutex);
        pthread_exit(NULL);
    }
    if (pthread_mutex_unlock(&buffer->mutex))
        pthread_exit(NULL);
}

void *buffer_pop(struct Buffer *buffer) {
    if (pthread_mutex_lock(&buffer->mutex))
        pthread_exit(NULL);
    while (buffer->len == 0) {
        if (pthread_cond_wait(&buffer->empty_cond, &buffer->mutex))
            pthread_exit(NULL);
    }
    void *elem = buffer->data[buffer->start];
    buffer->start = (buffer->start + 1) % buffer->cap;
    buffer->len--;
    if (pthread_cond_signal(&buffer->full_cond)) {
        pthread_mutex_unlock(&buffer->mutex);
        pthread_exit(NULL);
    }
    if (pthread_mutex_unlock(&buffer->mutex))
        pthread_exit(NULL);
    return elem;
}

void buffer_free(struct Buffer *buffer) {
    pthread_mutex_destroy(&buffer->mutex);
    pthread_cond_destroy(&buffer->full_cond);
    pthread_cond_destroy(&buffer->empty_cond);
    free(buffer);
}
