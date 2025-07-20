#ifndef TESTS_H
#define TESTS_H

void testGetBodyFromHttpRequest(void);
void testGetBodyFromHttpRequestWithUnixNewlines(void);
void testGetBodyFromHttpRequestNull(void);
void testGetBodyFromHttpRequestNoBody(void);

void testGetHttpVerbFromRequest(void);
void testGetHttpVerbFromRequestEdgeCases(void);

void testGetHttpUriFromRequest(void);
void testGetHttpUriFromRequestWithPath(void);
void testGetHttpUriFromRequestWithQuery(void);

void testCreateHttpHeaderDateString(void);

void testCreateContentLengthLine(void);

void testCreateResponse200(void);
void testCreateResponse404(void);
void testCreateResponse500(void);
void testCreateResponseOctetStream(void);

#endif
