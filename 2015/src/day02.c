#include "2015.h"

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> // strtol


static void
swap_ints(int *a, int *b)
{
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}


static void
sort_ints(int *dims)
{
    if (dims[0] > dims[1])
    {
        swap_ints(dims + 0, dims + 1);
    }
    if (dims[1] > dims[2])
    {
        swap_ints(dims + 1, dims + 2);
    }
}


static bool
is_digit(char c)
{
    bool result = (c >= '0') && (c <= '9');
    return result;
}


static int
parse_int(const char *input, const char **next)
{
    int result = 0;
    while (is_digit(*input))
    {
        char c = *input++;
        assert(result <= (INT_MAX / 10));
        result *= 10;
        result += c - '0';
    }

    *next = input;
    return result;
}


static const char *
parse_dimension(const char *input, int *dims)
{
    assert (isdigit(*input));

    const char *next;
    dims[0] = parse_int(input, &next);
    assert(*next == 'x');
    input = ++next;

    dims[1] = parse_int(input, &next);
    assert(*next == 'x');
    input = ++next;

    dims[2] = parse_int(input, &next);
    assert(!isdigit(*next));
    input = next;

    sort_ints(dims);

    while (*input && !isdigit(*input))
    {
        ++input;
    }

    return input;
}


static int
part1(const char *input)
{
    int result = 0;

    while (*input)
    {
        int dims[3];
        input = parse_dimension(input, dims);

        // wrapping paper
        int small = dims[0] * dims[1];
        int medium = dims[1] * dims[2];
        int large = dims[2] * dims[0];

        result += (3 * small) + (2 * medium) + (2 * large);
    }

    return result;
}


static int
part2(const char *input)
{
    int result = 0;

    while (*input)
    {
        int dims[3];
        input = parse_dimension(input, dims);

        // ribbons
        result += (2 * dims[0]) + (2 * dims[1]) + (dims[0] * dims[1] * dims[2]);
    }

    return result;
}


void
day02(const char *input)
{
#if 0
    typedef struct Test
    {
        const char *input;
        int result;
    } Test;

    const Test tests[] = {
        { "2x3x4", 58 },
        { "1x1x10", 43 },
        { "2x3x4\n1x1x10", 101 },
    };

    for (size_t i = 0; i < ARRAY_SIZE(tests); ++i)
    {
        const Test *test = tests + i;
        int result = day02(test->input);
        printf("test %zu: %s: expected = %d, actual = %d\n", i, test->input, test->result, result);
    }
#endif
    puts("\nDay 02:");

    int result = part1(input);
    assert(result == 1586300);
    printf("The elves need to order %d square feet of wrapping paper.\n", result);

    result = part2(input);
    assert(result == 3737498);
    printf("The elves need to order %d feet of ribbon.\n", result);
}
