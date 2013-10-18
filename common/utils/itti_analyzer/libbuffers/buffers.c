#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../rc.h"
#include "buffers.h"

extern int debug_buffers;

buffer_list_t list = {
    .head = NULL,
    .tail = NULL,
    .count = 0,
};
pthread_mutex_t buffer_list_mutex = PTHREAD_MUTEX_INITIALIZER;

static
int buffer_fetch(buffer_t *buffer, uint32_t offset, int size, void *value);

/* Try to fetch 8 bits unsigned from the buffer */
uint8_t buffer_get_uint8_t(buffer_t *buffer, uint32_t offset)
{
    uint8_t value;

    buffer_fetch(buffer, offset, 1, &value);

    return value;
}

/* Try to fetch 16 bits unsigned from the buffer */
uint16_t buffer_get_uint16_t(buffer_t *buffer, uint32_t offset)
{
    uint16_t value;

    buffer_fetch(buffer, offset, 2, &value);

    return value;
}

/* Try to fetch 32 bits unsigned from the buffer */
uint32_t buffer_get_uint32_t(buffer_t *buffer, uint32_t offset)
{
    uint32_t value;

    buffer_fetch(buffer, offset, 4, &value);

    return value;
}

static
int buffer_fetch(buffer_t *buffer, uint32_t offset, int size, void *value)
{
    if (buffer == NULL || value == NULL)
        return -1;
    if (size <= 0)
        return -1;

    if (buffer->size_bytes < ((offset >> 3) + size)) {
        printf("Not enough data to fetch\n");
        return -1;
    }

    memcpy(value, &buffer->data[offset >> 3], size);
    buffer->buffer_current = &buffer->data[(offset >> 3) + size];

    return 0;
}

int buffer_fetch_bits(buffer_t *buffer, uint32_t offset, int nbits, uint32_t *value)
{
    uint32_t temp = 0;
    int i;

    if (buffer == NULL || value == NULL)
        return RC_BAD_PARAM;

    /* We cannot fetch more than 32 bits */
    if (nbits > 32)
        return RC_BAD_PARAM;

    for (i = 0; i < nbits; i++)
    {
        temp |= ((buffer->data[(offset + i) / 8] >> ((offset + i) % 8)) & 1) << i;
    }

    *value = temp;

    return RC_OK;
}

/**
 * @brief Create a new buffer from data
 * @param buffer caller reference where created buffer will be stored
 * @param data Data to attach to the new buffer
 * @param length Length of data buffer
 * @param data_static flag that indicates if data pointer has been statically (= 0) or not (!= 0)
 */
int buffer_new_from_data(buffer_t **buffer, uint8_t *data, const uint32_t length,
                         int data_static)
{
    buffer_t *new;

    if (!buffer)
        return RC_BAD_PARAM;

    new = malloc(sizeof(buffer_t));

    new->size_bytes = length;
    if (data && length > 0) {
        if (data_static == 0) {
            new->data = malloc(sizeof(uint8_t) * new->size_bytes);
            memcpy(new->data, data, new->size_bytes);
        } else {
            new->data = data;
        }
        new->buffer_current = &new->data[0];
    } else {
        new->buffer_current = NULL;
    }

    new->next = NULL;

    *buffer = new;

    return 0;
}

#define INDENTED(fILE, x, y)            \
do {                                    \
    int indentation = x;                \
    while(indentation--) fprintf(fILE, " ");  \
    y;                                  \
} while(0)

#define INDENT_BREAK    20

void buffer_dump(buffer_t *buffer, FILE *to)
{
    FILE *file = to;
    uint32_t i;

    if (!buffer)
        return;
    if (!to)
        to = stdout;

    fprintf(file, "<Buffer>\n");
    INDENTED(file, 4, fprintf(file, "<Length>%u<Length>\n", buffer->size_bytes));
    INDENTED(file, 4, fprintf(file, "<Bytes>\n"));
    for (i = 0; i < buffer->size_bytes; i++)
    {
        if ((i % INDENT_BREAK) == 0)
            fprintf(file, "        ");
        fprintf(file, "0x%02x ", buffer->data[i]);
        if ((i % INDENT_BREAK) == (INDENT_BREAK - 1))
            fprintf(file, "\n");
    }
    if ((i % INDENT_BREAK) != (INDENT_BREAK - 1))
        fprintf(file, "\n");
    INDENTED(file, 4, fprintf(file, "</Bytes>\n"));
    fprintf(file, "</Buffer>\n");
}

int buffer_append_data(buffer_t *buffer, const uint8_t *data, const uint32_t length)
{
    if (!buffer)
        return -1;

    if (data && length > 0) {
        if (!buffer->data) {
            buffer->size_bytes = length;
            buffer->data = malloc(sizeof(uint8_t) * buffer->size_bytes);
            memcpy(buffer->data, data, buffer->size_bytes);
        } else {
            buffer->data = realloc(buffer->data, sizeof(uint8_t) * (buffer->size_bytes + length));
            memcpy(&buffer->data[buffer->size_bytes], data, length);
            buffer->size_bytes += length;
        }
        buffer->buffer_current = &buffer->data[0];
    }

    return 0;
}

int buffer_has_enouch_data(buffer_t *buffer, uint32_t offset, uint32_t to_get)
{
    int underflow;
    if (!buffer)
        return -1;
    underflow = (buffer->size_bytes >= ((offset + to_get) / 8)) ? 0 : -1;
    if (underflow && debug_buffers)
        printf("Detected Underflow offset %u, to_get %u, buffer size %u\n",
               offset, to_get, buffer->size_bytes);
    return underflow;
}

int buffer_get_from_mn(const uint32_t message_number, buffer_t **buffer)
{
    buffer_t *temp_buf;

    if (!buffer)
        return RC_BAD_PARAM;

    for (temp_buf = list.head; temp_buf; temp_buf = temp_buf->next) {
        if (temp_buf->message_number == message_number) {
            break;
        }
    }

    *buffer = temp_buf;

    return RC_OK;
}

int buffer_add_to_list(buffer_t *new_buf)
{
    if (!new_buf)
        return RC_BAD_PARAM;

    pthread_mutex_lock(&buffer_list_mutex);

    /* No element at tail */
    if (list.tail == NULL) {
        if (list.head == NULL) {
            list.head = new_buf;
            list.tail = list.head;
        } else {
            return RC_FAIL;
        }
    } else {
        list.tail->next = new_buf;
        new_buf->previous = list.tail;
        list.tail = new_buf;
    }

    list.count++;

    pthread_mutex_unlock(&buffer_list_mutex);

    return RC_OK;
}
