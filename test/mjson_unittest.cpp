#include "./external/win32_hypertest.cpp"
#include "../mjson.h"

TEST(MJSONTest, EmptyJSON_Object)
{
    char *JSONString = "{}";

    mjson_parser p;
    mjson_token t[1];

    mjson_init(&p);
    int MJSONResult = mjson_parse(&p, JSONString, strlen(JSONString), t, 1);

    // NOTE(Oskar): On success the count is returned.
    UNITTEST_ASSERT_TRUE(MJSONResult == 1);
    UNITTEST_ASSERT_TRUE(t[0].Type == MJSON_OBJECT);
    
}