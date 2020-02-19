#ifndef MJSON_H
#define MJSON_H

#ifndef MJSON_NO_STATIC
#define MJSON_API static
#else
#define MJSON_API extern
#endif

enum mjson_types
{
    MJSON_UNDEFINED,
    MJSON_OBJECT,
    MJSON_ARRAY,
    MJSON_STRING,
    MJSON_NUMBER,
    MJSON_BOOLEAN,
    MJSON_NULL
};

// TODO(Oskar): Store Start / End positions in JSON instead of actual value?
struct mjson_token
{
    mjson_type Type;
    char *Text;
    int32_t Value;
};

struct mjson_parser
{
    uint32_t Position;
    uint32_t TokenNext;
    int32_t TokenParent;
};

MJSON_API void
mjson_init(mjson_parser *Parser)
{
    Parser->Position = 0;
    Parser->TokenNext = 0;
    Parser->TokenParent = -1;
}

#endif // MJSON_H