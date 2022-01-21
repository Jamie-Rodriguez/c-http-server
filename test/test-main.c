#include "tests.h"
#include <unity.h>

void setUp(void) {}
void tearDown(void) {}

int main(void) {
        UNITY_BEGIN();
        RUN_TEST(testGetFirstLineFromHttpRequest);
        RUN_TEST(testGetBodyFromHttpRequest);
        RUN_TEST(testGetHttpVerbFromRequestLine);
        RUN_TEST(testGetHttpUriFromRequestLine);
        RUN_TEST(testCreateHttpHeaderDateString);
        RUN_TEST(testCreateContentLengthLine);
        RUN_TEST(testCreateResponse);
        return UNITY_END();
}