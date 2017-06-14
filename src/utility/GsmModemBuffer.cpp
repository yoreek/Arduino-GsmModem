#include "GsmModemBuffer.h"
#ifndef WITH_GSM_MODEM_DEBUG
#  undef WITH_DEBUG
#endif
#include <DebugUtil.h>

void GsmModemBuffer::reset() {
    last  = _lastLine = start;
    *last = '\0';
}

void GsmModemBuffer::put(uint8_t c) {
    *last++ = c;
    *last   = '\0';

    if (c == GM_LF) {
        _lastLine = last;
    }
}

/*
uint8_t GsmModemBuffer::peek(int pos) {
    if (pos >= 0)
        return start[pos];

    return last[pos];
}

char *GsmModemBuffer::nextLine(char **cur, size_t *len) {
    char *p, *p2;

    if (*cur == NULL)
        *cur = start;

    for (p = p2 = *cur; p2 < last; ++p2) {
        if (*p2 != '\n' && *p2 != '\r')
            continue;

        if (p == p2) {
            p++;
        }
        else {
            if ((p2 - p) > 0) {
                *len = p2 - p;
                *cur = p2 + 1;
                return p;
            }
            p = p2 + 1;
        }
    }

    *cur = p;

    return NULL;
}
*/

void GsmModemBuffer::removeLine(char *start, char *end) {
    //DEBUG("remove line: \"%#*s\"", end - start, start);

    while (end < last && (*end == GM_LF || *end == GM_CR))
        ++end;

    while (end < last)
        *start++ = *end++;

    last  = start;
    *last = '\0';

    if (_lastLine > last)
        _lastLine = last;
}

void GsmModemBuffer::begin(void) {
    start = (char *) malloc(_bufSize + 1);
    if (start == NULL) {
        //DEBUG("malloc failed");
        while (1)
            ;
    }
    end    = start + _bufSize;
    last   = _lastLine =  start;
    *start = '\0';
}
