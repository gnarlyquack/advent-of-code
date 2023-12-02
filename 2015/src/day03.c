#include "2015.h"

// stdlib
#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> // calloc


typedef union Position
{
    struct
    {
        int16_t x;
        int16_t y;
    };
    int32_t paired;
} Position;


typedef struct House
{
    Position position;
    uint32_t hash;
    bool visited;
} House;


typedef struct Grid
{
    uint32_t used;
    uint32_t capacity;
    House *houses;
} Grid;

#define GRID_MAX_LOAD 80
#define GRID_INIT_CAPACITY 8


static bool
is_power_of_two(uint32_t value)
{
    bool result = (value & (value - 1)) == 0;
    return result;
}


static void
init_grid(Grid *grid)
{
    grid->used = 0;
    grid->capacity = GRID_INIT_CAPACITY;
    grid->houses = calloc(8, sizeof(*grid->houses));
    assert(grid->houses);
}


static void
delete_grid(Grid *grid)
{
    free(grid->houses);
    grid->used = 0;
    grid->capacity = 0;
}


static void
grow_grid(Grid *grid)
{
    assert(grid->capacity < (UINT32_MAX / 2));
    uint32_t capacity = grid->capacity * 2;
    uint32_t mask = capacity - 1;

    House *houses = calloc(capacity, sizeof(*grid->houses));
    assert(houses);

    for (uint32_t i = 0; i < grid->capacity; ++i)
    {
        House *from = grid->houses + i;
        if (from->visited)
        {
            uint32_t index = from->hash & mask;
            House *to = houses + index;
            while (to->visited)
            {
                index = (index + 1) & mask;
                to = houses + index;
            }
            *to = *from;
        }
    }

    free(grid->houses);
    grid->capacity = capacity;
    grid->houses = houses;
}


static uint32_t
hash_number(int32_t value)
{
    // Hash using FNV-1a hash
    union
    {
        unsigned char bytes[4];
        int32_t word;
    } data = { .word = value };

    uint32_t hash = 2166136261; // FNV offset basis
    for (size_t i = 0; i < sizeof(data.bytes); ++i)
    {
        hash ^= data.bytes[i];
        hash *= 16777619; // FNV prime
    }

    return hash;
}


static void
visit_house(Grid *grid, Position position)
{
    uint32_t hash = hash_number(position.paired);

    assert(is_power_of_two(grid->capacity));
    uint32_t mask = grid->capacity - 1;
    uint32_t index = hash & mask;

    House *house = grid->houses + index;
    while (house->visited && (house->position.paired != position.paired))
    {
        index = (index + 1) & mask;
        house = grid->houses + index;
    }

    assert(!house->visited || (house->position.paired == position.paired));
    if (!house->visited)
    {
        house->position = position;
        house->hash = hash;
        house->visited = true;

        ++grid->used;
        if (((grid->used * 100) / grid->capacity) >= GRID_MAX_LOAD)
        {
            grow_grid(grid);
        }
    }
}


static void
move(Position *position, char command)
{
    assert(position->x <= INT16_MAX);
    assert(position->x >= INT16_MIN);
    assert(position->y <= INT16_MAX);
    assert(position->y >= INT16_MIN);

    position->x += (int16_t)((command == '>') - (command == '<'));
    position->y += (int16_t)((command == '^') - (command == 'v'));
}


static uint32_t
part1(const char *input)
{
    Grid grid;
    init_grid(&grid);

    Position santa = { .x = 0, .y = 0 };
    visit_house(&grid, santa);

    while (*input)
    {
        char c = *input++;
        move(&santa, c);
        visit_house(&grid, santa);
    }

    uint32_t result = grid.used;
    delete_grid(&grid);

    return result;
}


static uint32_t
part2(const char *input)
{
    Grid grid;
    init_grid(&grid);

    Position santa = { .x = 0, .y = 0 };
    Position robosanta = { .x = 0, .y = 0 };
    visit_house(&grid, santa);

    Position *this = &santa;
    Position *next = &robosanta;

    while (*input)
    {
        char c = *input++;
        move(this, c);
        visit_house(&grid, *this);

        Position *temp = this;
        this = next;
        next = temp;
    }

    uint32_t result = grid.used;
    delete_grid(&grid);

    return result;
}


void
day03(const char *input)
{
    puts("\nDay 03:");

    uint32_t result = part1(input);
    assert(result == 2081);
    printf("Santa delivers presents to %" PRIu32 " houses.\n", result);

    result = part2(input);
    assert(result == 2341);
    printf("Santa and Robo-Santa deliver presents to %" PRIu32 " houses.\n", result);
}
