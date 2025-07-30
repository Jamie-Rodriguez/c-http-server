#include "http-parsing.h"
#include "tests.h"
#include <assert.h>
#include <string.h>
#include <unity.h>

/* I used the cURL command below to create this request:
        curl --verbose \
             --request POST http://localhost:8080 \
             --header 'Content-Type: application/json' \
             --data '{"login":"my_login","password":"my_password"}'
*/
static const char* request =
    "POST / HTTP/1.1\r\n"
    "Host: localhost:8080\r\n"
    "User-Agent: curl/7.64.1\r\n"
    "Accept: */*\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: 45\r\n"
    "\r\n"
    "{\"login\":\"my_login\",\"password\":\"my_password\"}\r\n";

static const char* requestWithUnixNewlines = "GET / HTTP/1.1\n"
                                             "Host: localhost:8080\n"
                                             "User-Agent: curl/7.64.1\n"
                                             "\n"
                                             "Body content here\n";

void testGetBodyFromHttpRequest(void) {
        const char* body = getBodyFromHttpRequest(request);
        TEST_ASSERT_NOT_NULL(body);
        TEST_ASSERT_EQUAL_STRING(
            "{\"login\":\"my_login\",\"password\":\"my_password\"}\r\n",
            body);
}

void testGetBodyFromHttpRequestWithUnixNewlines(void) {
        const char* body = getBodyFromHttpRequest(requestWithUnixNewlines);
        TEST_ASSERT_NOT_NULL(body);
        TEST_ASSERT_EQUAL_STRING("Body content here\n", body);
}

void testGetBodyFromHttpRequestNull(void) {
        const char* body = getBodyFromHttpRequest(NULL);
        TEST_ASSERT_NULL(body);
}

void testGetBodyFromHttpRequestNoBody(void) {
        const char* requestNoBody =
            "GET / HTTP/1.1\r\nHost: localhost:8080\r\n";
        const char* body = getBodyFromHttpRequest(requestNoBody);
        TEST_ASSERT_NULL(body);
}

void testGetHttpVerbFromRequest(void) {
        TEST_ASSERT_EQUAL(GET, getHttpVerbFromRequest("GET /test HTTP/1.1"));
        TEST_ASSERT_EQUAL(POST, getHttpVerbFromRequest("POST /test HTTP/1.1"));
        TEST_ASSERT_EQUAL(UNKNOWN,
                          getHttpVerbFromRequest("INVALID /test HTTP/1.1"));
        TEST_ASSERT_EQUAL(UNKNOWN,
                          getHttpVerbFromRequest("PUT /test HTTP/1.1"));
        TEST_ASSERT_EQUAL(UNKNOWN,
                          getHttpVerbFromRequest("DELETE /test HTTP/1.1"));
}

void testGetHttpVerbFromRequestEdgeCases(void) {
        TEST_ASSERT_EQUAL(UNKNOWN, getHttpVerbFromRequest(""));
        TEST_ASSERT_EQUAL(UNKNOWN,
                          getHttpVerbFromRequest("GETX /test HTTP/1.1"));
        TEST_ASSERT_EQUAL(UNKNOWN,
                          getHttpVerbFromRequest("POS /test HTTP/1.1"));
        TEST_ASSERT_EQUAL(UNKNOWN, getHttpVerbFromRequest("NoSpaceHere"));
}

void testGetHttpUriFromRequest(void) {
        const char* uri = getHttpUriFromRequest("GET /test HTTP/1.1");
        TEST_ASSERT_NOT_NULL(uri);
        TEST_ASSERT_EQUAL_STRING("/test HTTP/1.1", uri);
}

void testGetHttpUriFromRequestWithPath(void) {
        const char* uri = getHttpUriFromRequest("POST /api/v1/users HTTP/1.1");
        TEST_ASSERT_NOT_NULL(uri);
        TEST_ASSERT_EQUAL_STRING("/api/v1/users HTTP/1.1", uri);
}

void testGetHttpUriFromRequestWithQuery(void) {
        const char* uri =
            getHttpUriFromRequest("GET /search?q=test&page=1 HTTP/1.1");
        TEST_ASSERT_NOT_NULL(uri);
        TEST_ASSERT_EQUAL_STRING("/search?q=test&page=1 HTTP/1.1", uri);
}

void testCreateHttpHeaderDateString(void) {
        char result[0x40];
        memset(result, 0, sizeof(result));

        struct tm time = { .tm_sec = 0,
                           .tm_min = 0,
                           .tm_hour = 0,
                           .tm_mday = 3,
                           .tm_mon = 11,
                           .tm_year = 91,
                           .tm_isdst = 0 };

        createHttpHeaderDateString(mktime(&time) - timezone,
                                   result,
                                   sizeof(result));

        TEST_ASSERT_EQUAL_STRING("Date: Tue, 03 Dec 1991 00:00:00 GMT", result);
}

void testCreateResponse200(void) {
        char result[0x100];
        memset(result, 0, sizeof(result));

        struct tm time = { .tm_sec = 0,
                           .tm_min = 0,
                           .tm_hour = 0,
                           .tm_mday = 3,
                           .tm_mon = 11,
                           .tm_year = 91,
                           .tm_isdst = 0 };

        const response successResponse = { .status = 200,
                                           .body = "{\"success\":\"true\"}",
                                           .bodyContentLen = 18,
                                           .time = mktime(&time) - timezone,
                                           .mime = JSON };

        const char* expectedSuccessResult =
            "HTTP/1.1 200 OK\r\n"
            "Date: Tue, 03 Dec 1991 00:00:00 GMT\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: 18\r\n"
            "Server: Jamie's Server (1.0.0)\r\n"
            "\r\n"
            "{\"success\":\"true\"}";

        createResponse(successResponse, result, sizeof(result));
        TEST_ASSERT_EQUAL_STRING(expectedSuccessResult, result);
}

void testCreateResponse404(void) {
        char result[0x100];
        memset(result, 0, sizeof(result));

        struct tm time = { .tm_sec = 0,
                           .tm_min = 0,
                           .tm_hour = 0,
                           .tm_mday = 3,
                           .tm_mon = 11,
                           .tm_year = 91,
                           .tm_isdst = 0 };

        const response notFoundResponse = { .status = 404,
                                            .body = "Not found.",
                                            .bodyContentLen = 10,
                                            .time = mktime(&time) - timezone,
                                            .mime = PLAIN_TEXT };

        const char* expectedNotFoundResult =
            "HTTP/1.1 404 Not Found\r\n"
            "Date: Tue, 03 Dec 1991 00:00:00 GMT\r\n"
            "Content-Type: text/plain; charset=UTF-8\r\n"
            "Content-Length: 10\r\n"
            "Server: Jamie's Server (1.0.0)\r\n"
            "\r\n"
            "Not found.";

        createResponse(notFoundResponse, result, sizeof(result));
        TEST_ASSERT_EQUAL_STRING(expectedNotFoundResult, result);
}

void testCreateResponse500(void) {
        char result[0x100];
        memset(result, 0, sizeof(result));

        struct tm time = { .tm_sec = 0,
                           .tm_min = 0,
                           .tm_hour = 0,
                           .tm_mday = 3,
                           .tm_mon = 11,
                           .tm_year = 91,
                           .tm_isdst = 0 };

        const response errorResponse = { .status = 500,
                                         .body = "Internal error",
                                         .bodyContentLen = 14,
                                         .time = mktime(&time) - timezone,
                                         .mime = PLAIN_TEXT };

        const char* expectedErrorResult =
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Date: Tue, 03 Dec 1991 00:00:00 GMT\r\n"
            "Content-Type: text/plain; charset=UTF-8\r\n"
            "Content-Length: 14\r\n"
            "Server: Jamie's Server (1.0.0)\r\n"
            "\r\n"
            "Internal error";

        createResponse(errorResponse, result, sizeof(result));
        TEST_ASSERT_EQUAL_STRING(expectedErrorResult, result);
}

void testCreateResponseOctetStream(void) {
        char result[0x100];
        memset(result, 0, sizeof(result));

        struct tm time = { .tm_sec = 0,
                           .tm_min = 0,
                           .tm_hour = 0,
                           .tm_mday = 3,
                           .tm_mon = 11,
                           .tm_year = 91,
                           .tm_isdst = 0 };

        const response binaryResponse = {
                .status = 200,
                .body = "\x01\x02\x03\x04",
                .bodyContentLen = 4,
                .time = mktime(&time) - timezone,
                .mime = 999 // Unknown MIME type to trigger default
        };

        const char* expectedBinaryResult =
            "HTTP/1.1 200 OK\r\n"
            "Date: Tue, 03 Dec 1991 00:00:00 GMT\r\n"
            "Content-Type: application/octet-stream\r\n"
            "Content-Length: 4\r\n"
            "Server: Jamie's Server (1.0.0)\r\n"
            "\r\n"
            "\x01\x02\x03\x04";

        createResponse(binaryResponse, result, sizeof(result));
        TEST_ASSERT_EQUAL_STRING(expectedBinaryResult, result);
}
