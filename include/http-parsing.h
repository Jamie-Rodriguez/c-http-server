#ifndef HTTP_PARSING_H
#define HTTP_PARSING_H

#include <stddef.h>
#include <time.h>


#define MAX_REQUEST_LEN 0x100
#define MAX_REQUEST_LINE_LEN 0x40

enum httpVerb { UNKNOWN = 0, GET = 1, POST = 2 };

enum mimeType { JSON = 0, PLAIN_TEXT = 1 };

typedef struct response {
        unsigned int status;
        enum mimeType mime;
        size_t bodyContentLen;
        time_t time;
        const char *body;
} response;

void getFirstLineFromHttpRequest(const char *request,
                                 char *dest,
                                 size_t destLen);
void getBodyFromHttpRequest(const char *request, char *dest, size_t destLen);
enum httpVerb getHttpVerbFromRequestLine(const char *requestLine);
void getHttpUriFromRequestLine(const char *requestLine,
                               char *dest,
                               size_t destLen);

void createHttpHeaderDateString(time_t time, char *dest, size_t destLen);
void createContentLengthLine(unsigned int n, char *dest, size_t destLen);

void createResponse(response response, char *dest, size_t destLen);

#endif
