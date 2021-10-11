#ifndef HTTPRequest_h

#define HTTPRequest_h

typedef enum HTTPMethods
{
    GET,
    POST,
    PUT,
    HEAD,
    DELETE,
    PATCH,
    CONNECT,
    OPTIONS,
    TRACE
}HTTPMethods;

typedef struct HTTPRequest
{
    int Method;
    char *URI;
    float HTTPVersion;
}HTTPRequest;

HTTPRequest HTTPRequest_constructor(char *request_str);


#endif