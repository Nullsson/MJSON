#ifndef HYPERTEST_HTTP_H
#define HYPERTEST_HTTP_H
#pragma once

struct unittest_http_request
{
    char ServerLocalAddress[46]; // NOTE: POSIX defines INET6_ADDRSTRLEN to 46.
    u32 ServerLocalPort;

    char *RequestMethod; // NOTE: "GET", "POST", etc ...
    char *URI; // NOTE: URL-decoded URI
    char *HttpVersion; // NOTE: "1.0" or "1.1"
    char *QueryString;
    char *RequestBody;
    u32 RequestBodySize;

    u32 NumberOfHeaders;
    struct unittest_http_request_header
    {
        char *Name; // NOTE: HTTP header name.
        char *Value; // NOTE: HTTP header value.
    } HttpHeaders[16]; // NOTE: Max 16 headers.
};

struct unittest_http_response
{
    char *HttpVersion;
    char *StatusCode;
    char *StatusMessage;

    char *ResponseBody; // NOTE: As is right now this has to be free'd as well.
    u32 ResponseBodySize;

    u32 NumberOfHeaders;
    struct unittest_http_request_header
    {
        char *Name; // NOTE: HTTP header name.
        char *Value; // NOTE: HTTP header value.
    } HttpHeaders[16]; // NOTE: Max 16 headers.

    char *BufferPointer; // NOTE: This is placed here so the user can free the allocated buffer.
};

#endif // HYPERTEST_HTTP_H