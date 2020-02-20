#ifndef MJSON_H
#define MJSON_H

#include <stdint.h>

#ifndef MJSON_NO_STATIC
#define MJSON_API static
#else
#define MJSON_API extern
#endif

enum mjson_type
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
    int Start;
    int End;
    int Size;
};

struct mjson_parser
{
    uint32_t Position;
    uint32_t TokenNext;
    int32_t TokenParent;
};

static mjson_token *mjson_alloc_token(mjson_parser *parser, mjson_token *tokens,
                                   const size_t num_tokens) {
  mjson_token *tok;
  if (parser->TokenNext >= num_tokens) {
    return NULL;
  }
  tok = &tokens[parser->TokenParent++];
  tok->Start = tok->End = -1;
  tok->Size = 0;

  return tok;
}

static void mjson_fill_token(mjson_token *token, const mjson_type type,
                            const int start, const int end) {
  token->Type = type;
  token->Start = start;
  token->End = end;
  token->Size = 0;
}

static int mjson_parse_primitive(mjson_parser *parser, const char *js,
                             const size_t len, mjson_token *tokens,
                             const size_t num_tokens)
{
    return -1;
}

static int mjson_parse_string(mjson_parser *parser, const char *js,
                             const size_t len, mjson_token *tokens,
                             const size_t num_tokens) {
  mjson_token *token;

  int start = parser->Position;

  parser->Position++;

  /* Skip starting quote */
  for (; parser->Position < len && js[parser->Position] != '\0'; parser->Position++) {
    char c = js[parser->Position];

    /* Quote: end of string */
    if (c == '\"') {
      if (tokens == NULL) {
        return 0;
      }
      token = mjson_alloc_token(parser, tokens, num_tokens);
      if (token == NULL) {
        parser->Position = start;
        return -1;
      }
      mjson_fill_token(token, MJSON_STRING, start + 1, parser->Position);

      return 0;
    }

    /* Backslash: Quoted symbol expected */
    if (c == '\\' && parser->Position + 1 < len) {
      int i;
      parser->Position++;
      switch (js[parser->Position]) {
      /* Allowed escaped symbols */
      case '\"':
      case '/':
      case '\\':
      case 'b':
      case 'f':
      case 'r':
      case 'n':
      case 't':
        break;
      /* Allows escaped symbol \uXXXX */
      case 'u':
        parser->Position++;
        for (i = 0; i < 4 && parser->Position < len && js[parser->Position] != '\0';
             i++) {
          /* If it isn't a hex character we have an error */
          if (!((js[parser->Position] >= 48 && js[parser->Position] <= 57) ||   /* 0-9 */
                (js[parser->Position] >= 65 && js[parser->Position] <= 70) ||   /* A-F */
                (js[parser->Position] >= 97 && js[parser->Position] <= 102))) { /* a-f */
            parser->Position = start;
            return -1;
          }
          parser->Position++;
        }
        parser->Position--;
        break;
      /* Unexpected symbol */
      default:
        parser->Position = start;
        return -1;
      }
    }
  }
  parser->Position = start;
  return -1;
}

MJSON_API int
mjson_parse(mjson_parser *Parser, char *Json, uint32_t JsonLength, mjson_token *Tokens, uint32_t TokenLength)
{
    int32_t Result = 0;
    int32_t Index = 0;
    mjson_token *CurrentToken;
    int32_t Count = Parser->TokenNext;

    for (; Parser->Position < JsonLength && Json[Parser->Position] != '\0'; ++Parser->Position)
    {
        char C = Json[Parser->Position];
        mjson_type Type;

        switch (C)
        {
            case '{':
            case '[':
            {
                Count++;
                if (Tokens == NULL)
                {
                    break;
                }

                CurrentToken = mjson_alloc_token(Parser, Tokens, TokenLength);
                if (CurrentToken == NULL)
                {
                    // TODO(Oskar): ERROR NO MEMORY
                    return -1;
                }

                if (Parser->TokenParent != -1)
                {
                    mjson_token *T = &Tokens[Parser->TokenParent];

                    // TODO(Oskar): Strict mode?

                    if (T->Type == MJSON_OBJECT)
                    {
                        // TODO(Oskar): INVALID JSON
                        return -1;
                    }

                    T->Size++;
                }

                CurrentToken->Type = (C == '{' ? MJSON_OBJECT : MJSON_ARRAY);
                CurrentToken->Start = Parser->Position;
                Parser->TokenParent = Parser->TokenNext - 1;
            } break;

            case '}':
            case ']':
            {
                if (Tokens == NULL)
                {
                    break;
                }

                Type = (C == '}' ? MJSON_OBJECT : MJSON_ARRAY);

                for (Index = Parser->TokenNext - 1; Index >= 0; --Index)
                {
                    CurrentToken = &Tokens[Index];
                    if (CurrentToken->Start != -1 && CurrentToken->End == -1)
                    {
                        if (CurrentToken->Type != Type)
                        {
                            // TODO(Oskar): INVALID JSON
                            return -1;
                        }

                        Parser->TokenParent = -1;
                        CurrentToken->End = Parser->Position + 1;
                        break;
                    }
                }

                // NOTE(Oskar): Unmatched closing bracket is an error
                if (Index == -1)
                {
                    // TODO(Oskar): INVALID JSON
                    return -1;
                }

                for (; Index >= 0; --Index)
                {
                    CurrentToken = &Tokens[Index];
                    if (CurrentToken->Start != -1 && CurrentToken->End == -1)
                    {
                        Parser->TokenParent = Index;
                        break;
                    }
                }
            } break;

            case '\"':
            {
                Result = mjson_parse_string(Parser, Json, JsonLength, Tokens, TokenLength);
                if (Result < 0)
                {
                    return Result;
                }

                Count++;
                if (Parser->TokenParent != -1 && Tokens != NULL)
                {
                    Tokens[Parser->TokenParent].Size++;
                }
            } break;
            case '\t':
            case '\r':
            case '\n':
            case ' ':
            {
            } break;

            case ':':
            {
                Parser->TokenParent = Parser->TokenNext - 1;
            } break;

            case ',':
            {
                // TODO(Oskar): ..
            } break;

            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case 't':
            case 'f':
            case 'n':
            {
                if (Tokens != NULL && Parser->TokenParent != -1)
                {
                    mjson_token *T = &Tokens[Parser->TokenParent];
                    if (T->Type == MJSON_OBJECT || (T->Type == MJSON_STRING && T->Size != 0))
                    {
                        // TODO(Oskar): INVALID JSON
                        return -1;
                    }
                }

                Result = mjson_parse_primitive(Parser, Json, JsonLength, Tokens, TokenLength);
                if (Result < 0)
                {
                    return Result;
                }
                Count++;
                if (Parser->TokenParent != -1 && Tokens != NULL)
                {
                    Tokens[Parser->TokenParent].Size++;
                }
            } break;

            default:
            {
                // TODO(Oskar): INVALID JSON
                return -1;
            }
        }
    }

    if (Tokens != NULL)
    {
        for (Index = Parser->TokenNext - 1; Index >= 0; --Index)
        {
            // NOTE(Oskar): Check for unmatched opening bracket.
            if (Tokens[Index].Start != -1 && Tokens[Index].End == -1)
            {
                // TODO(Oskar): Error type?
                return -1;
            }
        }
    }

    return Count;
}

MJSON_API void
mjson_init(mjson_parser *Parser)
{
    Parser->Position = 0;
    Parser->TokenNext = 0;
    Parser->TokenParent = -1;
}

#endif // MJSON_H