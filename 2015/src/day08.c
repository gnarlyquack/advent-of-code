#include "2015.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>

typedef struct Count
{
    int chars;
    int bytes;
    int recoded;
} Count;

static Count
count_bytes(const char *input)
{
    Count result = {0};
    int in_string = 0;
    while (*input)
    {
        if (input[0] == '"')
        {
            ++result.chars;
            result.recoded += 3;
            in_string = !in_string;
        }
        else if (in_string)
        {
            ++result.chars;
            ++result.bytes;
            ++result.recoded;
            if (input[0] == '\\')
            {
                switch (input[1])
                {
                    case '"':
                    case '\\':
                    {
                        ++result.chars;
                        result.recoded += 3;
                        ++input;
                    } break;

                    case 'x':
                    {
                        assert(isxdigit(input[2]));
                        assert(isxdigit(input[3]));
                        result.chars += 3;
                        result.recoded += 4;
                        input += 3;
                    } break;

                    default:
                    {
                        assert(0);
                    } break;
                }
            }
        }
        ++input;
    }

    return result;
}

void
day08(const char *input)
{
    puts("\nDay 08:");

#if 0
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))

    typedef struct Test
    {
        const char *input;
        int chars;
        int bytes;
        int recoded;
        int expected;
    } Test;


    Test tests[] = {
        {
            .input = "\"\"",
            .chars = 2,
            .bytes = 0,
            .recoded = 6,
            .expected = 2,
        },
        {
            .input = "\"abc\"",
            .chars = 5,
            .bytes = 3,
            .recoded = 9,
            .expected = 2,
        },
        {
            .input = "\"aaa\\\"aaa\"",
            .chars = 10,
            .bytes = 7,
            .recoded = 16,
            .expected = 3,
        },
        {
            .input = "\"\\x27\"",
            .chars = 6,
            .bytes = 1,
            .recoded = 11,
            .expected = 5,
        },
    };

    for (unsigned i = 0; i < ARRAY_SIZE(tests); ++i)
    {
        Test *test = tests + i;
        input = test->input;
        Count actual = count_bytes(input);
        assert(actual.chars == test->chars);
        assert(actual.bytes == test->bytes);
        assert(actual.recoded == test->recoded);
    }
#endif

    Count actual = count_bytes(input);
    int result = actual.chars - actual.bytes;
    assert(result == 1342);
    printf("The number of bytes of overhead in Santa's list is %d.\n", result);

    result = actual.recoded - actual.chars;
    assert(result == 2074);
    printf("The number of extra bytes of overhead to recode Santa's list is %d.\n", result);
}
