#include "2015.h"

// posix
#include <sys/stat.h>

// stdlib
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct Buffer
{
    size_t size;
    size_t capacity;
    char *data;
} Buffer;

static Buffer buffer;


static Buffer *
read_file(const char *filename)
{
    struct stat fileinfo;
    int status = stat(filename, &fileinfo);
    assert(status == 0);
    assert(S_ISREG(fileinfo.st_mode) && (fileinfo.st_size > 0));

    FILE *fh = fopen(filename, "r");
    assert(fh);

    buffer.size = (size_t)fileinfo.st_size;
    if (buffer.capacity < buffer.size)
    {
        buffer.data = realloc(buffer.data, buffer.size + 1);
        assert(buffer.data);
        buffer.capacity = buffer.size;
    }

    size_t bytes_read = fread(buffer.data, 1, buffer.size, fh);
    assert(bytes_read == buffer.size);
    buffer.data[buffer.size] = 0;

    fclose(fh);

    return &buffer;
}


int
main(void)
{
    Buffer *input = read_file("day01.txt");
    day01(input->data);

    input = read_file("day02.txt");
    day02(input->data);

    input = read_file("day03.txt");
    day03(input->data);

    day04();

    input = read_file("day05.txt");
    day05(input->data);

    input =read_file("day06.txt");
    day06(input->data);
}
