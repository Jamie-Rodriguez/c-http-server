#include "http-parsing.h"
#include <assert.h>
#include <string.h>
#include <unity.h>

/* I used the cURL command below to create this request:
        curl --verbose \
             --request POST http://localhost:8080 \
             --header 'Content-Type: application/json' \
             --data '{"login":"my_login","password":"my_password"}'
*/
const char *request =
    "POST / HTTP/1.1\r\n"
    "Host: localhost:8080\r\n"
    "User-Agent: curl/7.64.1\r\n"
    "Accept: */*\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: 45\r\n"
    "\r\n"
    "{\"login\":\"my_login\",\"password\":\"my_password\"}\r\n";

void testGetFirstLineFromHttpRequest() {
        char result[32];
        memset(result, 0, sizeof(result));
        getFirstLineFromHttpRequest(request, result, sizeof(result));

        TEST_ASSERT_EQUAL_STRING_LEN("POST / HTTP/1.1", result, sizeof(result));
}

void testGetBodyFromHttpRequest() {
        char result[64];
        memset(result, 0, sizeof(result));
        getBodyFromHttpRequest(request, result, sizeof(result));

        TEST_ASSERT_EQUAL_STRING_LEN(
            "{\"login\":\"my_login\",\"password\":\"my_password\"}",
            result,
            sizeof(result));
}

void testGetHttpVerbFromRequestLine() {
        TEST_ASSERT_TRUE(getHttpVerbFromRequestLine("GET /test HTTP/1.1") ==
                         GET);
        TEST_ASSERT_TRUE(getHttpVerbFromRequestLine("POST /test HTTP/1.1") ==
                         POST);
        TEST_ASSERT_TRUE(getHttpVerbFromRequestLine("INVALID /test HTTP/1.1") ==
                         UNKNOWN);
}

void testGetHttpUriFromRequestLine() {
        char result[8];
        memset(result, 0, sizeof(result));
        getHttpUriFromRequestLine("GET /test HTTP/1.1", result, sizeof(result));

        TEST_ASSERT_EQUAL_STRING_LEN("/test", result, sizeof(result));
}

void testCreateHttpHeaderDateString() {
        char result[64];
        memset(result, 0, sizeof(result));

        const struct tm time = {
                .tm_sec = 0,
                .tm_min = 0,
                .tm_hour = 0,
                .tm_mday = 3,
                .tm_mon = 11,
                .tm_year = 91,
        };

        createHttpHeaderDateString(timegm(&time), result, sizeof(result));

        TEST_ASSERT_EQUAL_STRING_LEN("Date: Tue, 03 Dec 1991 00:00:00 UTC",
                                     result,
                                     sizeof(result));
}

void testCreateContentLengthLine() {
        char result[32];
        memset(result, 0, sizeof(result));

        createContentLengthLine(1, result, sizeof(result));
        TEST_ASSERT_EQUAL_STRING_LEN("Content-Length: 1",
                                     result,
                                     sizeof(result));
        createContentLengthLine(12, result, sizeof(result));
        TEST_ASSERT_EQUAL_STRING_LEN("Content-Length: 12",
                                     result,
                                     sizeof(result));
        createContentLengthLine(123, result, sizeof(result));
        TEST_ASSERT_EQUAL_STRING_LEN("Content-Length: 123",
                                     result,
                                     sizeof(result));
}

void testCreateResponse() {
        char result[512];
        memset(result, 0, sizeof(result));

        const struct tm time = {
                .tm_sec = 0,
                .tm_min = 0,
                .tm_hour = 0,
                .tm_mday = 3,
                .tm_mon = 11,
                .tm_year = 91,
        };

        const response successResponse = { .status = 200,
                                           .body = "{\"success\":\"true\"}",
                                           .bodyContentLen = 18,
                                           .time = timegm(&time),
                                           .mime = JSON };

        const char *expectedSuccessResult =
            "HTTP/1.1 200 OK\r\n"
            "Date: Tue, 03 Dec 1991 00:00:00 UTC\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: 18\r\n"
            "Server: Jamie's Server (1.0.0)\r\n"
            "\r\n"
            "{\"success\":\"true\"}";

        createResponse(successResponse, result, sizeof(result));
        TEST_ASSERT_EQUAL_STRING_LEN(expectedSuccessResult,
                                     result,
                                     sizeof(result));


        memset(result, 0, sizeof(result));

        const response notFoundResponse = { .status = 404,
                                            .body = "Not found.",
                                            .bodyContentLen = 10,
                                            .time = timegm(&time),
                                            .mime = PLAIN_TEXT };

        const char *expectedNotFoundResult =
            "HTTP/1.1 404 Not Found\r\n"
            "Date: Tue, 03 Dec 1991 00:00:00 UTC\r\n"
            "Content-Type: text/plain; charset=UTF-8\r\n"
            "Content-Length: 10\r\n"
            "Server: Jamie's Server (1.0.0)\r\n"
            "\r\n"
            "Not found.";

        createResponse(notFoundResponse, result, sizeof(result));
        TEST_ASSERT_EQUAL_STRING_LEN(expectedNotFoundResult,
                                     result,
                                     sizeof(result));
}