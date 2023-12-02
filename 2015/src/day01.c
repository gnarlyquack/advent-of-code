#include "2015.h"

#include <assert.h>
#include <stdio.h>


static int
part1(const char *input)
{
    int floor = 0;
    for (char c = *input++; c; c = *input++)
    {
        floor += (c == '(') - (c == ')');
    }

    return floor;
}


static int
part2(const char *input)
{
    int floor = 0;
    int pos = 0;
    char c;
    while ((floor != -1) && (c = input[pos++]))
    {
        floor += (c == '(') - (c == ')');
    }

    return pos;
}


void
day01(const char *input)
{
    puts("Day 01:");

    int result = part1(input);
    assert(result == 74);
    printf("The instructions take Santa to floor %d.\n", result);

    result = part2(input);
    assert(result == 1795);
    printf("Santa reaches the basement at instruction %d.\n", result);
}
