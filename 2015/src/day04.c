#include "2015.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


typedef union Block
{
    char bytes[64];
    uint32_t words[16];
} Block;


static uint32_t T[] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,

    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,

    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,

    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
};

static const unsigned char pad_bytes[64] = { 0x80 };

static const uint32_t block_size = 64;
static const uint32_t block_mask = block_size - 1;


static void
hash_block(uint32_t *block, uint32_t *digest)
{
    uint32_t A = digest[0];
    uint32_t B = digest[1];
    uint32_t C = digest[2];
    uint32_t D = digest[3];

#define ROUND(a, b, c, d, k, s, i) \
    a += F(b, c, d) + block[k] + T[i]; \
    a = ROTATE_LEFT(a, s); \
    a += b

#define ROTATE_LEFT(value, amount) (value << amount) | (value >> (32 - amount))

    // round 1
#define F(x, y, z) ((x & y) | ((~x) & z))
    ROUND(A, B, C, D,  0,  7,  0);
    ROUND(D, A, B, C,  1, 12,  1);
    ROUND(C, D, A, B,  2, 17,  2);
    ROUND(B, C, D, A,  3, 22,  3);

    ROUND(A, B, C, D,  4,  7,  4);
    ROUND(D, A, B, C,  5, 12,  5);
    ROUND(C, D, A, B,  6, 17,  6);
    ROUND(B, C, D, A,  7, 22,  7);

    ROUND(A, B, C, D,  8,  7,  8);
    ROUND(D, A, B, C,  9, 12,  9);
    ROUND(C, D, A, B, 10, 17, 10);
    ROUND(B, C, D, A, 11, 22, 11);

    ROUND(A, B, C, D, 12,  7, 12);
    ROUND(D, A, B, C, 13, 12, 13);
    ROUND(C, D, A, B, 14, 17, 14);
    ROUND(B, C, D, A, 15, 22, 15);
#undef F

    // round 2
#define F(x, y, z) ((x & z) | (y & (~z)))
    ROUND(A, B, C, D,  1,   5,  16);
    ROUND(D, A, B, C,  6,   9,  17);
    ROUND(C, D, A, B, 11,  14,  18);
    ROUND(B, C, D, A,  0,  20,  19);

    ROUND(A, B, C, D,  5,   5,  20);
    ROUND(D, A, B, C, 10,   9,  21);
    ROUND(C, D, A, B, 15,  14,  22);
    ROUND(B, C, D, A,  4,  20,  23);

    ROUND(A, B, C, D,  9,   5,  24);
    ROUND(D, A, B, C, 14,   9,  25);
    ROUND(C, D, A, B,  3,  14,  26);
    ROUND(B, C, D, A,  8,  20,  27);

    ROUND(A, B, C, D, 13,   5,  28);
    ROUND(D, A, B, C,  2,   9,  29);
    ROUND(C, D, A, B,  7,  14,  30);
    ROUND(B, C, D, A, 12,  20,  31);
#undef F

    // round 3
#define F(x, y, z) (x ^ y ^ z)
    ROUND(A, B, C, D,  5,   4,  32);
    ROUND(D, A, B, C,  8,  11,  33);
    ROUND(C, D, A, B, 11,  16,  34);
    ROUND(B, C, D, A, 14,  23,  35);

    ROUND(A, B, C, D,  1,   4,  36);
    ROUND(D, A, B, C,  4,  11,  37);
    ROUND(C, D, A, B,  7,  16,  38);
    ROUND(B, C, D, A, 10,  23,  39);

    ROUND(A, B, C, D, 13,   4,  40);
    ROUND(D, A, B, C,  0,  11,  41);
    ROUND(C, D, A, B,  3,  16,  42);
    ROUND(B, C, D, A,  6,  23,  43);

    ROUND(A, B, C, D,  9,   4,  44);
    ROUND(D, A, B, C, 12,  11,  45);
    ROUND(C, D, A, B, 15,  16,  46);
    ROUND(B, C, D, A,  2,  23,  47);
#undef F

    // round 4
#define F(x, y, z) (y ^ (x | (~z)))
    ROUND(A, B, C, D,  0,   6,  48);
    ROUND(D, A, B, C,  7,  10,  49);
    ROUND(C, D, A, B, 14,  15,  50);
    ROUND(B, C, D, A,  5,  21,  51);

    ROUND(A, B, C, D, 12,   6,  52);
    ROUND(D, A, B, C,  3,  10,  53);
    ROUND(C, D, A, B, 10,  15,  54);
    ROUND(B, C, D, A,  1,  21,  55);

    ROUND(A, B, C, D,  8,   6,  56);
    ROUND(D, A, B, C, 15,  10,  57);
    ROUND(C, D, A, B,  6,  15,  58);
    ROUND(B, C, D, A, 13,  21,  59);

    ROUND(A, B, C, D,  4,   6,  60);
    ROUND(D, A, B, C, 11,  10,  61);
    ROUND(C, D, A, B,  2,  15,  62);
    ROUND(B, C, D, A,  9,  21,  63);
#undef F

#undef ROUND
#undef ROTATE_LEFT

    digest[0] += A;
    digest[1] += B;
    digest[2] += C;
    digest[3] += D;
}


static void
hash_md5(const char *input, size_t length, char *output)
{
    // The length in bits, truncated to 64 bits, is appended to the message.
    uint64_t bit_length = (8 * length) & 0xffffffffffffffff;

    // Pad the message with a '1' bit, some number of zero bits, and bit_length
    // so the message is an exact multiple of 64 bytes (512 bits). The '1' bit
    // and bit_length are always required.
    size_t length_padded = (length + 1 + sizeof(bit_length) + block_mask) & ~block_mask;
    size_t pad_length = length_padded - length - sizeof(bit_length);

    union
    {
        unsigned char bytes[16];
        uint32_t words[4];
    } digest;

    // initialization vectors
    digest.words[0] = 0x67452301;
    digest.words[1] = 0xefcdab89;
    digest.words[2] = 0x98badcfe;
    digest.words[3] = 0x10325476;

    Block block;
    while (length >= block_size)
    {
        memcpy(block.bytes, input, block_size);
        length -= block_size;
        input += block_size;
        hash_block(block.words, digest.words);
    }

    assert(length < block_size);
    memcpy(block.bytes, input, length);
    size_t remaining = block_size - length;

    size_t pad_offset = 0;
    if (pad_length > remaining)
    {
        memcpy(block.bytes + length, pad_bytes, remaining);
        length = 0;
        pad_offset += remaining;
        pad_length -= remaining;
        hash_block(block.words, digest.words);
    }

    memcpy(block.bytes + length, pad_bytes + pad_offset, pad_length);
    memcpy(block.bytes + length + pad_length, &bit_length, sizeof(bit_length));
    hash_block(block.words, digest.words);

    const char bin2hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    for (size_t i = 0, o = 0; i < sizeof(digest.bytes); ++i, o += 2)
    {
        unsigned char c = digest.bytes[i];
        output[o] = bin2hex[(c >> 4) & 0xf];
        output[o + 1] = bin2hex[c & 0xf];
    }
}


static void
append_digits(char *input, size_t length, uint32_t number, size_t digits)
{
    input[length + digits] = 0;
    while (digits--)
    {
        char c = (char)(number % 10) + '0';
        number /= 10;
        input[length + digits] = c;
    }
}


static uint32_t
mine_advent_coins(const char *secret, size_t length, uint32_t nzeroes)
{
    char input[128];
    assert(length < sizeof(input));
    memcpy(input, secret, length);

    uint32_t result = 0;
    size_t digits = 1;
    size_t next_digit = 10;

    char digest[32];

    uint32_t index;
    do
    {
        ++result;
        if (result == next_digit)
        {
            ++digits;
            assert((length + digits) < sizeof(input));
            next_digit *= 10;
        }
        append_digits(input, length, result, digits);

        hash_md5(input, length + digits, digest);
        index = 0;
        while ((digest[index] == '0') && (++index < nzeroes));
    } while (index < nzeroes);

    return result;
}


void
day04(void)
{
#if 0
    typedef struct Test
    {
        const char *input;
        const char *expected;
    } Test;

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))

    Test tests[] = {
        { .input = "abcdef609043", .expected = "000001dbbfa3a5c83a2d506429c7b00e" },
        { .input = "pqrstuv1048970", .expected = "000006136ef2ff3b291c85725f17325c" },
    };

    for (size_t i = 0; i < ARRAY_SIZE(tests); ++i)
    {
        Test *test = tests + i;
        hash_md5(test->input, strlen(test->input), actual);
        assert(strcmp(actual, test->expected) == 0);
    }
#endif

    puts("\nDay 04:");

    uint32_t nzeroes = 5;
    const char *input = "iwrupvqb";
    uint32_t result = mine_advent_coins(input, strlen(input), nzeroes);
    assert(result == 346386);
    printf("Santa's secret number for %u zeroes is %u.\n", nzeroes, result);

    nzeroes = 6;
    result = mine_advent_coins(input, strlen(input), nzeroes);
    assert(result == 9958218);
    printf("Santa's secret number for %u zeroes is %u.\n", nzeroes, result);
}
