#include "2015.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


typedef enum TokenType
{
    TOKEN_CONNECT,
    TOKEN_VALUE,
    TOKEN_WIRE,

    TOKEN_AND,
    TOKEN_LSHIFT,
    TOKEN_NOT,
    TOKEN_OR,
    TOKEN_RSHIFT,
} TokenType;


typedef struct Token
{
    TokenType type;
    union
    {
        uint16_t value;
        char wire[2];
    };
} Token;


typedef struct Parser
{
    const char *input;
    Token token;
} Parser;


typedef union Wire
{
    uint16_t value;
    char name[2];
} Wire;


typedef struct Source
{
    TokenType type;
    union
    {
        uint16_t value;
        Wire wire;
    };
} Source;


typedef struct Gate
{
    TokenType type;
    Source left;
    Source right;
} Gate;


typedef struct Connection
{
    Wire wire;
    union
    {
        TokenType type;
        Source source;
        Gate gate;
    };
} Connection;


typedef struct Circuit
{
    uint32_t used;
    uint32_t size;
    Connection *connections;
} Circuit;

#define CIRCUIT_INIT_SIZE 8
#define CIRCUIT_MAX_LOAD 80


static uint16_t
parse_u16(const char **input)
{
    const char *text = *input;
    uint16_t result = 0;
    while (isdigit(*text))
    {
        assert(result < (UINT16_MAX / 10));
        result *= 10;
        result += (uint16_t)(*text - '0');
        ++text;
    }

    *input = text;
    return result;
}


static const char *
parse_string(const char **input, const char *string)
{
    const char *result = *input;
    const char *text = result;

    while (*string && (*string == *text))
    {
        ++string;
        ++text;
    }

    if (*string || (*text && !isspace(*text)))
    {
        result = 0;
    }
    else
    {
        *input = text;
    }

    return result;
}

static uint32_t
hash_value(uint16_t value)
{
    // Hash using FNV-1a hash
    union
    {
        unsigned char bytes[2];
        uint16_t value;
    } data = { .value = value };

    uint32_t hash = 2166136261; // FNV offset basis
    for (uint32_t i = 0; i < sizeof(data.bytes); ++i)
    {
        hash ^= data.bytes[i];
        hash *= 16777619; // FNV prime
    }

    return hash;
}


static void
init_circuit(Circuit *circuit)
{
    circuit->used = 0;
    circuit->size = CIRCUIT_INIT_SIZE;
    circuit->connections = calloc(circuit->size, sizeof(*circuit->connections));
}


static void
delete_circuit(Circuit *circuit)
{
    free(circuit->connections);
}


static void
grow_circuit(Circuit *circuit)
{
    uint32_t size = circuit->size * 2;
    uint32_t mask = size - 1;
    Connection *connections = calloc(size, sizeof(*connections));

    for (uint32_t i = 0 ; i < circuit->size; ++i)
    {
        Connection *from = circuit->connections + i;
        if (from->type)
        {
            uint32_t hash = hash_value(from->wire.value);
            uint32_t index = hash & mask;
            Connection *to = connections + index;
            while (to->type)
            {
                index = (index + 1) & mask;
                to = connections + index;
            }
            *to = *from;
        }
    }

    free(circuit->connections);
    circuit->connections = connections;
    circuit->size = size;
}


static Connection *
find_connection(Circuit *circuit, Wire wire)
{
    uint32_t hash = hash_value(wire.value);
    uint32_t mask = circuit->size - 1;
    uint32_t index = hash & mask;

    Connection *result = circuit->connections + index;
    while (result->type && (result->wire.value != wire.value))
    {
        index = (index + 1) & mask;
        result = circuit->connections + index;
    }

    return result;
}


static void
add_connection(Circuit *circuit, Connection *connection)
{
    Connection *dest = find_connection(circuit, connection->wire);
    circuit->used += dest->wire.value != connection->wire.value;
    *dest = *connection;
    if ((circuit->used * 100 / circuit->size) >= CIRCUIT_MAX_LOAD)
    {
        grow_circuit(circuit);
    }
}


static uint16_t lookup_source(Circuit *circuit, Source source);


static uint16_t
lookup_signal(Circuit *circuit, Wire wire)
{
    Connection *connection = find_connection(circuit, wire);
    assert(connection->wire.value == wire.value);

    uint16_t result = 0;
    switch (connection->type)
    {
        case TOKEN_VALUE:
        {
            result = connection->source.value;
        } break;

        case TOKEN_WIRE:
        {
            result = lookup_signal(circuit, connection->source.wire);
            connection->type = TOKEN_VALUE;
            connection->source.value = result;
        } break;

        case TOKEN_NOT:
        {
            result = ~lookup_source(circuit, connection->gate.left);
            connection->type = TOKEN_VALUE;
            connection->source.value = result;
        } break;

        case TOKEN_AND:
        {
            uint16_t left = lookup_source(circuit, connection->gate.left);
            uint16_t right = lookup_source(circuit, connection->gate.right);
            result = left & right;
            connection->type = TOKEN_VALUE;
            connection->source.value = result;
        } break;

        case TOKEN_OR:
        {
            uint16_t left = lookup_source(circuit, connection->gate.left);
            uint16_t right = lookup_source(circuit, connection->gate.right);
            result = left | right;
            connection->type = TOKEN_VALUE;
            connection->source.value = result;
        } break;

        case TOKEN_LSHIFT:
        {
            uint16_t left = lookup_source(circuit, connection->gate.left);
            uint16_t right = connection->gate.right.value;
            result = left << right;
            connection->type = TOKEN_VALUE;
            connection->source.value = result;
        } break;

        case TOKEN_RSHIFT:
        {
            uint16_t left = lookup_source(circuit, connection->gate.left);
            uint16_t right = connection->gate.right.value;
            result = left >> right;
            connection->type = TOKEN_VALUE;
            connection->source.value = result;
        } break;

        default:
        {
            assert(false);
        } break;
    }

    return result;
}


static uint16_t
lookup_source(Circuit *circuit, Source source)
{
    uint16_t result;
    if (source.type == TOKEN_VALUE)
    {
        result = source.value;
    }
    else
    {
        result = lookup_signal(circuit, source.wire);
    }

    return result;
}


static void
parse_token(Parser *parser)
{
    while (isspace(*parser->input))
    {
        ++parser->input;
    }

    if (isdigit(*parser->input))
    {
        parser->token.type = TOKEN_VALUE;
        parser->token.value = parse_u16(&parser->input);
    }
    else
    {
        switch (*parser->input)
        {
            case 'A':
            {
                ++parser->input;
                parser->token.type = TOKEN_AND;
                const char *parsed = parse_string(&parser->input, "ND");
                assert(parsed);
            } break;

            case 'L':
            {
                ++parser->input;
                parser->token.type = TOKEN_LSHIFT;
                const char *parsed = parse_string(&parser->input, "SHIFT");
                assert(parsed);
            } break;

            case 'N':
            {
                ++parser->input;
                parser->token.type = TOKEN_NOT;
                const char *parsed = parse_string(&parser->input, "OT");
                assert(parsed);
            } break;

            case 'O':
            {
                ++parser->input;
                parser->token.type = TOKEN_OR;
                const char *parsed = parse_string(&parser->input, "R");
                assert(parsed);
            } break;

            case 'R':
            {
                ++parser->input;
                parser->token.type = TOKEN_RSHIFT;
                const char *parsed = parse_string(&parser->input, "SHIFT");
                assert(parsed);
            } break;

            case '-':
            {
                ++parser->input;
                parser->token.type = TOKEN_CONNECT;
                const char *parsed = parse_string(&parser->input, ">");
                assert(parsed);
            } break;

            default:
            {
                parser->token.type = TOKEN_WIRE;

                assert(islower(parser->input[0]));
                parser->token.wire[0] = parser->input[0];

                if (islower(parser->input[1]))
                {
                    parser->token.wire[1] = parser->input[1];
                    parser->input += 2;
                }
                else
                {
                    parser->token.wire[1] = 0;
                    parser->input += 1;
                }
            } break;
        }
    }
}


static void
parse_instructions(const char *input, Circuit *circuit)
{
    Parser parser;
    parser.input = input;

    while (*parser.input)
    {
        Connection connection;
        parse_token(&parser);

        if (parser.token.type == TOKEN_NOT)
        {
            connection.type = TOKEN_NOT;

            parse_token(&parser);
            assert((parser.token.type == TOKEN_VALUE) || (parser.token.type == TOKEN_WIRE));
            connection.gate.left.type = parser.token.type;
            connection.gate.left.value = parser.token.value;

        }
        else
        {
            assert((parser.token.type == TOKEN_VALUE) || (parser.token.type == TOKEN_WIRE));
            Source left;
            left.type = parser.token.type;
            left.value = parser.token.value;

            parse_token(&parser);
            if (parser.token.type == TOKEN_CONNECT)
            {
                connection.source = left;
            }
            else
            {
                connection.type = parser.token.type;
                connection.gate.left = left;

                parse_token(&parser);
                assert((parser.token.type == TOKEN_VALUE) || (parser.token.type == TOKEN_WIRE));
                connection.gate.right.type = parser.token.type;
                connection.gate.right.value = parser.token.value;
            }
        }

        if (parser.token.type != TOKEN_CONNECT)
        {
            parse_token(&parser);
        }
        assert(parser.token.type == TOKEN_CONNECT);

        parse_token(&parser);
        assert(parser.token.type == TOKEN_WIRE);
        connection.wire.value = parser.token.value;

        add_connection(circuit, &connection);

        parser.input += (*parser.input == '\n');
    }
}


void
day07(const char *input)
{
    puts("\nDay 07:");


    Circuit circuit;
    init_circuit(&circuit);

    parse_instructions(input, &circuit);

    Wire wire = { .value = 'a' };
    uint16_t result = lookup_signal(&circuit, wire);
    assert(result == 46065);
    printf("Circuit 'a' has signal: %u\n", result);

    parse_instructions(input, &circuit);

    wire.value = 'b';
    Connection *connection = find_connection(&circuit, wire);
    connection->type = TOKEN_VALUE;
    connection->source.value = result;

    wire.value = 'a';
    result = lookup_signal(&circuit, wire);
    assert(result == 14134);
    printf("After overriding circuit 'b', circuit 'a' has signal: %u\n", result);

    delete_circuit(&circuit);
}
