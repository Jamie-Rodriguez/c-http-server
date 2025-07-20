#include "tests.h"
#include <unity.h>

void setUp(void) {}
void tearDown(void) {}

int main(void) {
        UNITY_BEGIN();

        RUN_TEST(testGetBodyFromHttpRequest);
        RUN_TEST(testGetBodyFromHttpRequestWithUnixNewlines);
        RUN_TEST(testGetBodyFromHttpRequestNull);
        RUN_TEST(testGetBodyFromHttpRequestNoBody);

        RUN_TEST(testGetHttpVerbFromRequest);
        RUN_TEST(testGetHttpVerbFromRequestEdgeCases);

        RUN_TEST(testGetHttpUriFromRequest);
        RUN_TEST(testGetHttpUriFromRequestWithPath);
        RUN_TEST(testGetHttpUriFromRequestWithQuery);

        RUN_TEST(testCreateHttpHeaderDateString);

        RUN_TEST(testCreateContentLengthLine);

        RUN_TEST(testCreateResponse200);
        RUN_TEST(testCreateResponse404);
        RUN_TEST(testCreateResponse500);
        RUN_TEST(testCreateResponseOctetStream);

        return UNITY_END();
}
