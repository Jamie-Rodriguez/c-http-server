#ifndef TESTS_H
#define TESTS_H

void testGetFirstLineFromHttpRequest(void);
void testGetBodyFromHttpRequest(void);
void testGetHttpVerbFromRequestLine(void);
void testGetHttpUriFromRequestLine(void);

void testCreateHttpHeaderDateString(void);
void testCreateContentLengthLine(void);
void testCreateResponse(void);

#endif
