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

TEST(MJSONTest, EmptyJSON_Array)
{
    char *JSONString = "[]";

    mjson_parser p;
    mjson_token t[1];

    mjson_init(&p);
    int MJSONResult = mjson_parse(&p, JSONString, strlen(JSONString), t, 1);

    UNITTEST_ASSERT_TRUE(MJSONResult == 1);
    UNITTEST_ASSERT_TRUE(t[0].Type == MJSON_ARRAY);
}

TEST(MJSONTest, JSONObject_Simple)
{
    char *JSONString = "{\"key\":0}";

    mjson_parser p;
    mjson_token t[3];

    mjson_init(&p);
    int MJSONResult = mjson_parse(&p, JSONString, strlen(JSONString), t, 3);

    UNITTEST_ASSERT_TRUE(MJSONResult == 3);
    UNITTEST_ASSERT_TRUE(t[0].Type == MJSON_OBJECT);
    
    UNITTEST_ASSERT_TRUE(t[1].Type == MJSON_STRING);
    UNITTEST_ASSERT_TRUE(strncmp(JSONString + t[1].Start, "key", 3) == 0);

    UNITTEST_ASSERT_TRUE(t[2].Type == MJSON_PRIMITIVE);
    UNITTEST_ASSERT_TRUE(strncmp(JSONString + t[2].Start, "0", 1) == 0);
}

TEST(MJSONTest, JSONObject_Advanced)
{
    char *JSONString =
        "{\"menu\":" 
            "{"
                "\"id\": \"file\","
                "\"value\": \"File\","
                "\"popup\":" 
                    "{"
                        "\"menuitem\":" 
                        "["
                            "{\"value\": \"New\", \"onclick\": \"CreateNewDoc()\"},"
                            "{\"value\": \"Open\", \"onclick\": \"OpenDoc()\"},"
                            "{\"value\": \"Close\", \"onclick\": \"CloseDoc()\"}"
                        "]"
                "}"
            "}"
        "}";

    mjson_parser p;
    mjson_token t[26];

    mjson_init(&p);
    int MJSONResult = mjson_parse(&p, JSONString, strlen(JSONString), t, 26);

    UNITTEST_ASSERT_TRUE(MJSONResult == 26);

    UNITTEST_ASSERT_TRUE(t[0].Type == MJSON_OBJECT); // {
    UNITTEST_ASSERT_TRUE(t[1].Type == MJSON_STRING); // menu
    UNITTEST_ASSERT_TRUE(t[2].Type == MJSON_OBJECT); // {
    UNITTEST_ASSERT_TRUE(t[3].Type == MJSON_STRING); // id
    UNITTEST_ASSERT_TRUE(t[4].Type == MJSON_STRING); // file
    UNITTEST_ASSERT_TRUE(t[5].Type == MJSON_STRING); // value
    UNITTEST_ASSERT_TRUE(t[6].Type == MJSON_STRING); // File
    UNITTEST_ASSERT_TRUE(t[7].Type == MJSON_STRING); // popup
    UNITTEST_ASSERT_TRUE(t[8].Type == MJSON_OBJECT); // {
    UNITTEST_ASSERT_TRUE(t[9].Type == MJSON_STRING); // menuitem
    UNITTEST_ASSERT_TRUE(t[10].Type == MJSON_ARRAY); // [

    UNITTEST_ASSERT_TRUE(t[11].Type == MJSON_OBJECT); // {
    UNITTEST_ASSERT_TRUE(t[12].Type == MJSON_STRING); // value
    UNITTEST_ASSERT_TRUE(t[13].Type == MJSON_STRING); // New
    UNITTEST_ASSERT_TRUE(t[14].Type == MJSON_STRING); // onclick
    UNITTEST_ASSERT_TRUE(t[15].Type == MJSON_STRING); // CreateNewDoc()

    UNITTEST_ASSERT_TRUE(t[16].Type == MJSON_OBJECT); // {
    UNITTEST_ASSERT_TRUE(t[17].Type == MJSON_STRING); // value
    UNITTEST_ASSERT_TRUE(t[18].Type == MJSON_STRING); // Open
    UNITTEST_ASSERT_TRUE(t[19].Type == MJSON_STRING); // onclick
    UNITTEST_ASSERT_TRUE(t[20].Type == MJSON_STRING); // OpenDoc()

    UNITTEST_ASSERT_TRUE(t[21].Type == MJSON_OBJECT); // {
    UNITTEST_ASSERT_TRUE(t[22].Type == MJSON_STRING); // value
    UNITTEST_ASSERT_TRUE(t[23].Type == MJSON_STRING); // Close
    UNITTEST_ASSERT_TRUE(t[24].Type == MJSON_STRING); // onclick
    UNITTEST_ASSERT_TRUE(t[25].Type == MJSON_STRING); // CloseDoc()
}