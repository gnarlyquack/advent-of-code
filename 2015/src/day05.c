#include "2015.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>


#define ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))

// Rijndael S-box, because why not?
static const uint8_t sbox[] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
    0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
    0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
    0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
    0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
    0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
    0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
    0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
    0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
    0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
    0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
    0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16,
};


static int
part1(const char *input)
{
#define NICE 0x7 // i.e., 0b111

    int result = 0;
    while (*input)
    {
        int nvowels = 0;
        // we start with no unallowed character pairs
        int32_t nice = 1 << 1;
        char last = 0;

        for (char c = *input; (c >= 'a') && (c <= 'z'); c = *++input)
        {
            switch (c)
            {
                case 'a':
                case 'e':
                case 'i':
                case 'o':
                case 'u':
                {
                    ++nvowels;
                    nice |= nvowels >= 3;
                } break;

                // check for unallowed character pairs
                case 'b':
                {
                    nice &= ~((last == 'a') << 1);
                } break;

                case 'd':
                {
                    nice &= ~((last == 'c') << 1);
                } break;

                case 'q':
                {
                    nice &= ~((last == 'p') << 1);
                } break;

                case 'y':
                {
                    nice &= ~((last == 'x') << 1);
                } break;
            }

            nice |= (c == last) << 2;
            last = c;
        }

        result += nice == NICE;
        input += *input == '\n';
    }

    return result;

#undef NICE
}


static int
part2(const char *input)
{
#define NICE 0x3 // i.e, 0b11

    typedef struct Pair
    {
        uint16_t value;
        uint16_t index;
    } Pair;

    int result = 0;
    while (*input)
    {
        Pair pairs[32] = {0};
        int32_t nice = 0;
        char prev2 = 0;
        char prev1 = 0;
        uint16_t index = 0;

        for (char c = *input; (c >= 'a') && (c <= 'z'); c = *++input)
        {
            ++index;

            // check if string has repeat letter (separated by another)
            nice |= (prev2 == c);

            // Hash pairs using a Pearson hash.
            // Since we're only dealing with a small number of small values
            // (i.e., 16 2-byte strings), this hash seems useful because it's
            // simple, quick, and hashes to a small output space. The hash
            // itself doesn't specify a substution table (s-box), so we just
            // grab Rjindael's (apparently used in AES).
            uint16_t value = (uint16_t)(prev1 | (c << 8));
            uint8_t hash = 0;
            hash = sbox[hash ^ prev1];
            hash = sbox[hash ^ c];
            hash &= ARRAY_SIZE(pairs) - 1;

            Pair *pair = pairs + hash;
            while (pair->index && (pair->value != value))
            {
                hash = (hash + 1) & (ARRAY_SIZE(pairs) - 1);
                pair = pairs + hash;
            }

            if (pair->index)
            {
                assert(pair->value == value);
                // check if string has repeat, non-overlapping pair
                nice |= ((index - pair->index) > 1) << 1;
            }
            else
            {
                pair->value = value;
                pair->index = index;
            }

            prev2 = prev1;
            prev1 = c;
        }

        result += nice == NICE;
        input += *input == '\n';
    }

    return result;

#undef NICE
}


void
day05(const char *input)
{
    puts("\nDay 05:");

    int result = part1(input);
    assert(result == 258);
    printf("%d strings are nice.\n", result);

    result = part2(input);
    assert(result == 53);
    printf("%d new strings are nice.\n", result);

#if 0
    typedef struct Test
    {
        const char *input;
        bool expected;
    } Test;


    Test tests[] = {
        { .input = "qjhvhtzxzqqjkmpb", .expected = true },
        { .input = "xxyxx", .expected = true },
        { .input = "uurcxstgmygtbstg", .expected = false },
        { .input = "ieodomkazucvgmuy", .expected = false },
    };

    for (unsigned i = 0; i < ARRAY_SIZE(tests); ++i)
    {
        Test *test = tests + i;
        bool actual = part2(test->input);
        assert(actual == test->expected);
    }
#endif
}
