#include "http_stream.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "facil.h"

#define CONTENT_LEN "Content-Length"

http_stream::http_stream()
    : m_bInHdr(1), m_ContentLength(0), m_ContentRead(0), m_type(NONE) {}

http_stream::~http_stream() {}

void http_stream::reset(void) {
  if (debug)
    printf("Restting stream\n");
  m_bInHdr = 1;
  m_ContentLength = 0;
  m_ContentRead = 0;
  m_type = NONE;
}

void http_stream::input(const char *buf, int len) {
  int i;
  int j;
  int hdr_end = 0;
  /*
   * Check for the end of the header in here
   */
  if (m_bInHdr) {
    if (m_type == NONE) {
      if (len >= 3 && !strncasecmp("GET", buf, 3))
        m_type = GET;
      else if (len >= 4 && !strncasecmp("POST", buf, 4))
        m_type = POST;
      else if (len >= 3 && !strncasecmp("PUT", buf, 3))
        m_type = PUT;
      else if (len >= 3 && !strncasecmp("DELETE", buf, 6))
        m_type = DELETE;
      else {
        m_type = UNKNOWN;
      }
    }
    for (i = 0; i < len; i++) {
      if (i + 1 < len) {
        if (buf[i] == '\n' && buf[i + 1] == '\n') {
          m_bInHdr = 0;
          hdr_end = i + 2;
          break;
        }
      }
      if (i + 3 < len) {
        if (buf[i] == '\r' && buf[i + 1] == '\n' && buf[i + 2] == '\r' &&
            buf[i + 3] == '\n') {
          m_bInHdr = 0;
          hdr_end = i + 4;
          break;
        }
      }
    }
    if (!m_ContentLength) {
      if (i > strlen(CONTENT_LEN)) {
        for (j = 0; j < i - strlen(CONTENT_LEN); j++) {
          if (!strncasecmp(buf + j, CONTENT_LEN, strlen(CONTENT_LEN))) {
            m_ContentLength = atoi(buf + j + strlen(CONTENT_LEN) + +2);
            break;
          }
        }
      }
    }
  }
  if (debug && hdr_end) {
    printf("HDR:\n");
    fwrite(buf, 1, hdr_end, stdout);
  }
  if (m_ContentLength) {
    if (debug)
      printf("I found Content Length %d\n", m_ContentLength);
    if (hdr_end)
      m_ContentRead += len - hdr_end;
    else
      m_ContentRead += len;

    if (debug)
      printf("have read %d / %d\n", m_ContentRead, m_ContentLength);
  }
}

int http_stream::is_stream_end(void) {
  /* stream is at an end if we are past the header and
     there is a content length and we have read it all.
     XXXPAM possibly also true if there is no content length ???
     */

  if (m_type == GET) {
    if (debug)
      printf("Checking for GET\n");
    return (!m_bInHdr &&
            (!m_ContentLength || m_ContentRead == m_ContentLength));
  } else if (m_type == DELETE) {
    if (debug)
      printf("Checking for DELETE\n");
    return (!m_bInHdr &&
            (!m_ContentLength || m_ContentRead == m_ContentLength));
  } else if (m_type == POST || m_type == PUT) {
    if (debug) {
      printf("Checking for POST/PUT\n");
      printf("m_bInHdr %d, m_ContentRead %d,"
             " m_ContentLength %d\n",
             m_bInHdr, m_ContentRead, m_ContentLength);
    }
    return (!m_bInHdr && m_ContentRead == m_ContentLength);
  }

  if (debug)
    printf("Checking for other\n");
  return (!m_bInHdr && m_ContentLength && m_ContentRead == m_ContentLength);
}
