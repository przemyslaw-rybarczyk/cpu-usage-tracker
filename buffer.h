#pragma once

#include "types.h"

struct Buffer;

struct Buffer *buffer_new(size_t capacity);
void buffer_push(struct Buffer *buffer, void *elem);
void *buffer_pop(struct Buffer *buffer);
void buffer_free(struct Buffer *buffer);
