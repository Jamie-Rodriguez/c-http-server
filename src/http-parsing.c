#include "http-parsing.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

const char *serverLine = "Server: Jamie's Server (1.0.0)";

void getFirstLineFromHttpRequest(const char *request,
                                 char *dest,
                                 size_t destLen) {
        // strtok() mutates the original string! Make a copy
        char str[MAX_REQUEST_LEN];
        memset(str, 0, sizeof(str));
        memccpy(str, request, '\0', sizeof(str));

        // Get the string before the first \r\n
        const char *line = strtok(str, "\r\n");

        memccpy(dest, line, '\0', destLen);
}

void getBodyFromHttpRequest(const char *request, char *dest, size_t destLen) {
        // strsep() mutates the original string! Make a copy
        char str[MAX_REQUEST_LEN];
        memset(str, 0, sizeof(str));
        memccpy(str, request, '\0', sizeof(str));

        // To avoid the incompatible pointer types error, use char **, not char (*)[]
        char *s = str;
        char *previousString;

        bool foundEmptyLine = false;
        while (s != NULL) {
                previousString = strsep(&s, "\n");

                // Process the body
                if (foundEmptyLine) {
                        // Check string and strip out carriage returns
                        // I do this by terminating the string when I find one
                        for (size_t i = 0; i < strlen(previousString); i++) {
                                if (previousString[i] == '\r')
                                        previousString[i] = 0;
                        }
                        memccpy(dest, previousString, '\0', destLen);
                        return;
                }

                // Found the empty line, next loop is the body
                if (strcmp(previousString, "\r") == 0) foundEmptyLine = true;
        }
}

enum httpVerb getHttpVerbFromRequestLine(const char *requestLine) {
        // strtok() mutates the original string! Make a copy
        char str[MAX_REQUEST_LINE_LEN];
        memset(str, 0, sizeof(str));
        memccpy(str, requestLine, '\0', sizeof(str));

        const char *verb = strtok(str, " ");

        // C doesn't allow switch statements on strings? Wot?
        if (strcmp(verb, "GET") == 0) {
                return GET;
        } else if (strcmp(verb, "POST") == 0) {
                return POST;
        } else {
                return UNKNOWN;
        }
}

void getHttpUriFromRequestLine(const char *requestLine,
                               char *dest,
                               size_t destLen) {
        // strtok() mutates the original string! Make a copy
        char str[MAX_REQUEST_LINE_LEN];
        memset(str, 0, sizeof(str));
        memccpy(str, requestLine, '\0', sizeof(str));

        // Assume the URI will be the second space-delimited token
        strtok(str, " ");
        const char *uri = strtok(NULL, " ");

        // Longest HTTP verb is length("CONNECT") = 7
        memccpy(dest, uri, '\0', destLen);
}

void createHttpHeaderDateString(time_t time, char *dest, size_t destLen) {
        // time_t conforms to time protocol as seen in RFC 868
        const struct tm tm = *gmtime(&time);
        strftime(dest, destLen, "Date: %a, %d %b %Y %H:%M:%S %Z", &tm);
}

void createContentLengthLine(unsigned int n, char *dest, size_t destLen) {
        snprintf(dest, destLen, "Content-Length: %d", n);
}

void createResponse(response response, char *dest, size_t destLen) {
        char statusLine[64];
        memset(statusLine, 0, sizeof(statusLine));
        char dateLine[64];
        memset(dateLine, 0, sizeof(dateLine));
        char contentTypeLine[64];
        memset(contentTypeLine, 0, sizeof(contentTypeLine));
        char contentLengthLine[32];
        memset(contentLengthLine, 0, sizeof(contentLengthLine));

        if (response.status == 200) {
                memccpy(statusLine,
                        "HTTP/1.1 200 OK",
                        '\0',
                        sizeof(statusLine));
        } else if (response.status == 404) {
                memccpy(statusLine,
                        "HTTP/1.1 404 Not Found",
                        '\0',
                        sizeof(statusLine));
        } else {
                memccpy(statusLine,
                        "HTTP/1.1 500 Internal Server Error",
                        '\0',
                        sizeof(statusLine));
        }

        createHttpHeaderDateString(response.time, dateLine, sizeof(dateLine));

        if (response.mime == JSON) {
                memccpy(contentTypeLine,
                        "Content-Type: application/json",
                        '\0',
                        sizeof(contentTypeLine));
        } else if (response.mime == PLAIN_TEXT) {
                memccpy(contentTypeLine,
                        "Content-Type: text/plain; charset=UTF-8",
                        '\0',
                        sizeof(contentTypeLine));
        } else {
                memccpy(contentTypeLine,
                        "Content-Type: application/octet-stream",
                        '\0',
                        sizeof(contentTypeLine));
        }

        createContentLengthLine(response.bodyContentLen,
                                contentLengthLine,
                                sizeof(contentLengthLine));

        snprintf(dest,
                 destLen,
                 "%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n\r\n%s",
                 statusLine,
                 dateLine,
                 contentTypeLine,
                 contentLengthLine,
                 serverLine,
                 response.body);
}
