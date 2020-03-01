#include "hypertest_http.h"

// TODO: Place within string library...
/*
    Skip the characters until one of the delimiter characters was found.
    0-terminate the resulting word. Skip the delimiter and following whitespaces if any.
    Advance pointer to buffer to the next word. Return found 0-terminated word.
    Delimiters can be quoted with the quotechar.
*/
OM_INTERNAL char *
SkipQuoted(char **Buffer, char *Delimiters, char *Whitespace, char QuoteChar)
{
    char *P;
    char *BeginWord;
    char *EndWord;
    char *EndWhitespace;
    
    BeginWord = *Buffer;
    EndWord = BeginWord + strcspn(BeginWord, Delimiters);
    
    if (EndWord > BeginWord)
    {
        P = EndWord - 1;
        
        while (*P == QuoteChar)
        {
            // NOTE: If there is anything beyond EndWord, copy it.
            if (*EndWord == '\0')
            {
                *P = '\0';
                break;
            }
            else
            {
                size_t EndOff = strcspn(EndWord + 1, Delimiters);
                memmove(P, EndWord, EndOff + 1); // TODO: Unsure about this call.. 
                P += EndOff;
                EndWord += EndOff + 1;
            }
        }
        
        for (P++; P < EndWord; P++)
        {
            *P = '\0';
        }
    }
    
    if (*EndWord == '\0')
    {
        *Buffer = EndWord;
    }
    else
    {
        EndWhitespace = EndWord + 1 + strspn(EndWord + 1, Whitespace);
        
        for (P = EndWord; P < EndWhitespace; P++)
        {
            *P = '\0';
        }
        
        *Buffer = EndWhitespace;
    }
    
    return (BeginWord);
}

// TODO: Place within string library...
OM_INTERNAL char *
Skip(char **Buffer, char *Delimiters)
{
    return (SkipQuoted(Buffer, Delimiters, Delimiters, 0));
}

OM_INTERNAL SOCKET 
InitiateConnection(char *ServerAddress, char *Port)
{
    WSADATA WSAData;
    SOCKET ConnectionSocket = INVALID_SOCKET;

    addrinfo *RequestResult = NULL;
    addrinfo *Pointer = NULL;
    addrinfo Hints;

    s32 Result;

    Result = WSAStartup(MAKEWORD(2,2), &WSAData);
    if (Result != 0)
    {
        printf("WSAStartup failed with error: %d\n", Result);
        return (NULL);
    }

    UNITTEST_ZeroStruct(Hints);
    Hints.ai_family = AF_UNSPEC;
    Hints.ai_socktype = SOCK_STREAM;
    Hints.ai_protocol = IPPROTO_TCP;

    // NOTE: Resolve the server address and port.
    Result = getaddrinfo(ServerAddress, Port, &Hints, &RequestResult);
    if (Result != 0)
    {
        printf("getaddrinfo failed with error: %d\n", Result);
        WSACleanup();
        return (NULL);
    }

    // NOTE: Attempting to connect to an address untill one succeeds
    for (Pointer = RequestResult; Pointer != NULL; Pointer = Pointer->ai_next)
    {
        // NOTE: Create a socket to use
        ConnectionSocket = socket(Pointer->ai_family, Pointer->ai_socktype, Pointer->ai_protocol);
        if (ConnectionSocket == INVALID_SOCKET)
        {
            printf("socket failed with error: %d\n", WSAGetLastError());
            WSACleanup();
            return (NULL);
        }

        // NOTE: Connect to the server
        Result = connect(ConnectionSocket, Pointer->ai_addr, (int)Pointer->ai_addrlen);
        if (Result == SOCKET_ERROR)
        {
            closesocket(ConnectionSocket);
            ConnectionSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(RequestResult);

    if (ConnectionSocket == INVALID_SOCKET)
    {
        printf("Unable to establish a connection to the server!\n");
        WSACleanup();
        return (NULL);
    }

    return (ConnectionSocket);
}

OM_INTERNAL void 
HttpRequestAddHeader(unittest_http_request *HttpRequest, char *Name, char *Value)
{
    HttpRequest->HttpHeaders[HttpRequest->NumberOfHeaders].Name = Name;
    HttpRequest->HttpHeaders[HttpRequest->NumberOfHeaders].Value = Value;
    ++HttpRequest->NumberOfHeaders;
}

OM_INTERNAL char * 
BuildHttpRequestString(unittest_http_request *HttpRequest)
{
    // TODO: Define a buffer length and decide to not use magic value?
    char *HttpRequestBuffer = (char *) malloc(sizeof(char) * 1024);

    // NOTE: Creating separate buffer for the variable number of Http Headers.
    //       An initial call to sprintf an empty string into the buffer is added
    //       in order to avoid the whole empty buffer to be copied into the resulting
    //       request when there is no Http Headers available.
    char *HttpHeaderBuffer = (char *) malloc(sizeof(char) * 512);
    sprintf(HttpHeaderBuffer, "");

    char *HttpHeaderBufferPointer = HttpHeaderBuffer;
    for (u32 Index = 0; Index < HttpRequest->NumberOfHeaders; ++Index)
    {
        HttpHeaderBufferPointer += sprintf(HttpHeaderBufferPointer, "%s: %s\r\n", 
            HttpRequest->HttpHeaders[Index].Name, 
            HttpRequest->HttpHeaders[Index].Value);
    }

    // NOTE: Building final HttpRequest string.
    sprintf(HttpRequestBuffer, 
        "%s %s HTTP/%s\r\n"
        "%s"
        "\r\n"
        "%s", 
        HttpRequest->RequestMethod, HttpRequest->URI, HttpRequest->HttpVersion,
        HttpHeaderBuffer, HttpRequest->RequestBody);

    free(HttpHeaderBuffer);
    return (HttpRequestBuffer);
}

OM_INTERNAL b32
IsValidHttpMethod(char * HttpMethod)
{
    b32 Result = false;

    Result = !strcmp(HttpMethod, "GET") || !strcmp(HttpMethod, "POST") ||
        !strcmp(HttpMethod, "DELETE") || !strcmp(HttpMethod, "OPTIONS");

    return (Result);
}

OM_INTERNAL b32
ParseHttpResponse(char *Buffer, unittest_http_response *HttpResponse)
{
    char BufferCopy[512];
    char *BufferCopyPointer = BufferCopy;
    strcpy(BufferCopy, Buffer);

    // NOTE: According to RFC all the initial whitespace should be ignored.
    while (*Buffer != '\0' && isspace(* (unsigned char *) Buffer))
    {
        Buffer++;
        BufferCopyPointer++;
    }

    HttpResponse->HttpVersion = Skip(&Buffer, " ");
    HttpResponse->StatusCode = Skip(&Buffer, " ");
    HttpResponse->StatusMessage = Skip(&Buffer, "\r\n");

    if ((strncmp(HttpResponse->HttpVersion, "HTTP/", 5) == 0))
    {
        HttpResponse->HttpVersion += 5; // NOTE: Skipping initial "HTTP/"

        // NOTE: Parsing HTTP Headers.
        for (u32 HeaderIndex = 0; HeaderIndex < OM_ARRAYCOUNT(HttpResponse->HttpHeaders); ++HeaderIndex)
        {
            HttpResponse->HttpHeaders[HeaderIndex].Name = SkipQuoted(&Buffer, ":", " ", 0);
            HttpResponse->HttpHeaders[HeaderIndex].Value = Skip(&Buffer, "\r\n");

            if (HttpResponse->HttpHeaders[HeaderIndex].Name[0] == '\0')
            {
                break;
            }

            HttpResponse->NumberOfHeaders = HeaderIndex + 1;
        }

        Skip(&BufferCopyPointer, "\r\n");
        for (u32 HeaderIndex = 0; HeaderIndex < HttpResponse->NumberOfHeaders-1; ++HeaderIndex)
        {
            Skip(&BufferCopyPointer, "\r\n");
        }

        HttpResponse->ResponseBody = (char *) malloc(sizeof(char) * 512);
        strcpy(HttpResponse->ResponseBody, BufferCopyPointer);
        HttpResponse->ResponseBodySize = strlen(HttpResponse->ResponseBody);

        return (true);
    }

    return (false);
}

OM_INTERNAL b32 
SendHttpRequest(SOCKET Socket, unittest_http_request *HttpRequest)
{
    char *RequestString = BuildHttpRequestString(HttpRequest);
    u32 RequestStringLength = strlen(RequestString);

    s32 Result = send(Socket, RequestString, RequestStringLength, 0);
    if (Result == SOCKET_ERROR)
    {
        printf("Send failed with error: %d\n", WSAGetLastError());
        closesocket(Socket);
        WSACleanup();
        return (false);
    }

    free(RequestString);
    return (true);
}

OM_INTERNAL b32
RecieveHttpRequest(SOCKET Socket, unittest_http_response *HttpResponse)
{
    // TODO: Define a buffer length and decide to not use magic value?
    char *Buffer = (char *) malloc(sizeof(char) * 512);
    s32 BufferLength = 512;
    s32 Result;

    do
    {
        Result = recv(Socket, Buffer, BufferLength, 0);
        if (Result > 0)
        {
            // NOTE: Bytes were recieved.
            Buffer[Result] = '\0';
        }
        else if (Result == 0)
        {
            // NOTE: Connection was closed
        }
        else
        {
            // NOTE: Recv failed with error.
            printf("Recv failed with error: %d\n", WSAGetLastError());
            closesocket(Socket);
            WSACleanup();
            return (false);
        }
        //printf("Result for current loop: %d\n", Result);
    } while (Result > 0);

    // NOTE: Uncomment this to view the whole response in order for easier debugging.
    //printf("\n%s\n", Buffer);

    // NOTE: Parsing the HTTP Response into the given datastructure.
    HttpResponse->BufferPointer = Buffer;
    if (ParseHttpResponse(Buffer, HttpResponse) == false)
    {
        // NOTE: Parse was unsuccessfull hence something went wrong.
        return (false);
    }

    return (true);
}

OM_INTERNAL void 
CloseConnection(SOCKET Socket)
{
    closesocket(Socket);
    WSACleanup();
}