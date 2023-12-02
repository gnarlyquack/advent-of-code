#include "2015.h"

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#define GRID_DIMENSION 1000


typedef enum Operation
{
    TURN_ON,
    TURN_OFF,
    TOGGLE,
} Operation;


typedef struct Position
{
    uint16_t x;
    uint16_t y;
} Position;


typedef struct Instruction
{
    Operation operation;
    Position from;
    Position to;
} Instruction;


static uint16_t
parse_number(const char **input)
{
    const char *code = *input;
    uint16_t result = 0;
    while (isdigit(*code))
    {
        assert(result < (UINT16_MAX / 10));
        result *= 10;
        result += (uint16_t)(*code - '0');
        ++code;
    }

    *input = code;
    return result;
}


static void
parse_position(const char **input, Position *position)
{
    position->x = parse_number(input);
    assert(**input == ',');
    *input += 1;
    position->y = parse_number(input);
}


static Operation
parse_operation(const char **input)
{
    const char *code = *input;
    assert(code[0] == 't');

    Operation result;
    switch (code[1])
    {
        case 'o':
        {
            assert(code[2] == 'g');
            assert(code[3] == 'g');
            assert(code[4] == 'l');
            assert(code[5] == 'e');
            code += 6;
            result = TOGGLE;
        } break;

        case 'u':
        {
            assert(code[2] == 'r');
            assert(code[3] == 'n');
            assert(code[4] == ' ');
            assert(code[5] == 'o');

            if (code[6] == 'n')
            {
                code += 7;
                result = TURN_ON;
            }
            else
            {
                assert(code[6] == 'f');
                assert(code[7] == 'f');
                code += 8;
                result = TURN_OFF;
            }
        } break;

        default:
        {
            assert(false);
        } break;
    }

    *input = code;
    return result;
}


static void
parse_instruction(const char **input, Instruction *instruction)
{
    instruction->operation = parse_operation(input);
    assert(**input == ' ');
    *input += 1;
    parse_position(input, &instruction->from);

    const char *code = *input;
    assert(code[0] == ' ');
    assert(code[1] == 't');
    assert(code[2] == 'h');
    assert(code[3] == 'r');
    assert(code[4] == 'o');
    assert(code[5] == 'u');
    assert(code[6] == 'g');
    assert(code[7] == 'h');
    assert(code[8] == ' ');
    code += 9;
    *input = code;

    parse_position(input, &instruction->to);
}


static int
part1(const char *input)
{
    char *grid = calloc(GRID_DIMENSION * GRID_DIMENSION, sizeof(*grid));
    assert(grid);

    while (*input)
    {
        const char **code = &input;
        Instruction instruction;
        parse_instruction(code, &instruction);

        for (uint32_t y = instruction.from.y; y <= instruction.to.y; ++y)
        {
            for (uint32_t x = instruction.from.x; x <= instruction.to.x; ++x)
            {
                size_t index = y * GRID_DIMENSION + x;
                switch (instruction.operation)
                {
                    case TURN_ON:
                    {
                        grid[index] |= 1;
                    } break;

                    case TOGGLE:
                    {
                        grid[index] ^= 1;
                    } break;

                    default:
                    {
                        assert(instruction.operation == TURN_OFF);
                        grid[index] &= ~1;
                    } break;
                }
            }
        }

        input += (*input == '\n');
    }

    int result = 0;
    for (uint32_t y = 0; y < GRID_DIMENSION; ++y)
    {
        for (uint32_t x = 0; x < GRID_DIMENSION; ++x)
        {
            uint32_t index = y * GRID_DIMENSION + x;
            result += grid[index];
        }
    }

    free(grid);
    return result;
}


static int
part2(const char *input)
{
    char *grid = calloc(GRID_DIMENSION * GRID_DIMENSION, sizeof(*grid));

    while (*input)
    {
        const char **code = &input;
        Instruction instruction;
        parse_instruction(code, &instruction);

        for (uint32_t y = instruction.from.y; y <= instruction.to.y; ++y)
        {
            for (uint32_t x = instruction.from.x; x <= instruction.to.x; ++x)
            {
                size_t index = y * GRID_DIMENSION + x;
                switch (instruction.operation)
                {
                    case TURN_ON:
                    {
                        assert(grid[index] < CHAR_MAX);
                        ++grid[index];
                    } break;

                    case TOGGLE:
                    {
                        assert(grid[index] < (CHAR_MAX - 1));
                        grid[index] += 2;
                    } break;

                    default:
                    {
                        assert(instruction.operation == TURN_OFF);
                        grid[index] = (char)(grid[index] - (grid[index] > 0));
                    } break;
                }
            }
        }

        input += (*input == '\n');
    }

    int result = 0;
    for (uint32_t y = 0; y < GRID_DIMENSION; ++y)
    {
        for (uint32_t x = 0; x < GRID_DIMENSION; ++x)
        {
            uint32_t index = y * GRID_DIMENSION + x;
            result += grid[index];
        }
    }

    free(grid);
    return result;
}


void
day06(const char *input)
{
    puts("\nDay 06:");

    int result = part1(input);
    assert(result == 543903);
    printf("%u lights are lit.\n", result);

    result = part2(input);
    assert(result == 14687245);
    printf("Total brightness is %u.\n", result);

#if 0
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))

    typedef struct Test
    {
        const char *input;
        uint32_t expected;
    } Test;


    Test tests[] = {
        {
            .input = "turn on 0,0 through 999,999",
            .expected = 1000000,
        },
        {
            .input = "toggle 0,0 through 999,0",
            .expected = 1000,
        },
        {
            .input = "turn off 499,499 through 500,500",
            .expected = 0,
        },
    };

    for (unsigned i = 0; i < ARRAY_SIZE(tests); ++i)
    {
        Test *test = tests + i;
        uint32_t actual = parse_instructions(test->input);
        assert(actual == test->expected);
    }
#endif

}
