#include <avr/pgmspace.h>
#include "GsmModemUtil.h"
#ifndef WITH_GSM_MODEM_DEBUG
#  undef WITH_DEBUG
#endif
#include <DebugUtil.h>

char *GsmModemUtil::match(const char *pattern, size_t pattern_len, char *str, size_t str_len, uint8_t matchingType) {
    //DEBUG("pattern: %s str: %s len: %d", pattern, str, str_len);
    if (pattern_len > str_len) {
        return NULL;
    }

    switch (matchingType) {
    case GM_MATCH_EQUALS:
        if (pattern_len == str_len && strncmp(pattern, str, pattern_len) == 0) {
            return str;
        }
        break;
    case GM_MATCH_CONTAINS:
        while (pattern_len <= str_len) {
            if (strncmp(pattern, str, pattern_len) == 0) {
                return str;
            }
            ++str;
        }
        break;
    case GM_MATCH_STARTS_WITH:
        if (strncmp(pattern, str, pattern_len) == 0) {
            return str;
        }
        break;
    }

    return NULL;
}

GM_CHAR *GsmModemUtil::strMatch(char *str1, size_t str1_len, GM_CHAR_ARRAY *patterns, uint8_t matchingType) {
    //DEBUG("str: %*s", str1_len, str1);
    while (*patterns != NULL) {
        //DEBUG("pattern: %s", *patterns);
        if (strMatch(str1, str1_len, *patterns, matchingType)) {
            //DEBUG("found: %s", *patterns);
            break;
        }
        patterns++;
    }

    return *patterns;
}

GM_CHAR *GsmModemUtil::strMatch_P(char *str1, size_t str1_len, GM_CHAR_ARRAY *patterns, uint8_t matchingType) {
    GM_CHAR *p = NULL;
    //DEBUG("str: %*s", str1_len, str1);
    while ((p = (GM_CHAR *) pgm_read_word_near(patterns)) != NULL) {
        //DEBUG("pattern: %P", p);
        if (strMatch_P(str1, str1_len, p, matchingType)) {
            //DEBUG("found: %P", p);
            break;
        }
        patterns++;
    }

    return p;
}

bool GsmModemUtil::strMatch(char *str1, size_t str1_len, GM_CHAR *str2, uint8_t matchingType) {
    switch (matchingType) {
        case GM_MATCH_EQUALS:
            return strEqual(str1, str1_len, str2);
        case GM_MATCH_STARTS_WITH:
            return strStartWith(str1, str1_len, str2);
        case GM_MATCH_CONTAINS:
            break;
    }

    return false;
}

bool GsmModemUtil::strMatch_P(char *str1, size_t str1_len, GM_CHAR *str2, uint8_t matchingType) {
    switch (matchingType) {
        case GM_MATCH_EQUALS:
            return strEqual_P(str1, str1_len, str2);
        case GM_MATCH_STARTS_WITH:
            return strStartWith_P(str1, str1_len, str2);
        case GM_MATCH_CONTAINS:
            break;
    }

    return false;
}

bool GsmModemUtil::strMatch(char *str1, size_t str1_len, const __FlashStringHelper *str2, uint8_t matchingType) {
    return strMatch_P(str1, str1_len, (GM_CHAR *) str2);
}

bool GsmModemUtil::strStartWith(char *str, ssize_t str_len, GM_CHAR *substr) {
    char c;

    if (str_len == 0)
        return *substr == '\0';

    while ((c = *substr++) != '\0') {
        if ((str_len >= 0 && str_len-- == 0) || *str++ != c)
            return false;
    }

    return true;
}

bool GsmModemUtil::strStartWith_P(char *str, ssize_t str_len, GM_CHAR *substr) {
    char c;

    if (str_len == 0)
        return pgm_read_byte_near(substr) == '\0';

    while ((c = pgm_read_byte_near(substr++)) != '\0') {
        if ((str_len >= 0 && str_len-- == 0) || *str++ != c)
            return false;
    }

    return true;
}

bool GsmModemUtil::strStartWith(char *str, ssize_t str_len, const __FlashStringHelper *substr) {
    return strStartWith_P(str, str_len, (GM_CHAR *) substr);
}

bool GsmModemUtil::strEqual(char *str1, ssize_t str1_len, GM_CHAR *str2) {
    char c;

    if (str1_len == 0)
        return *str2 == '\0';

    if (str1_len < 0) {
        do {
            c = *str2++;
            if (*str1++ != c)
                return false;
        } while (c != '\0');
    }
    else {
        do {
            if (str1_len-- == 0) {
                if ('\0' != *str2++)
                    return false;
            }
            else {
                if (*str1++ != *str2++)
                    return false;
            }
        } while (str1_len >= 0 && str2[-1] != '\0');
    }

    return true;
}

bool GsmModemUtil::strEqual_P(char *str1, ssize_t str1_len, GM_CHAR *str2) {
    char c;

    if (str1_len == 0)
        return pgm_read_byte_near(str2) == '\0';

    if (str1_len < 0) {
        do {
            c = pgm_read_byte_near(str2++);
            if (*str1++ != c)
                return false;
        } while (c != '\0');
    }
    else {
        do {
            c = pgm_read_byte_near(str2++);
            if (str1_len-- == 0) {
                if ('\0' != c)
                    return false;
            }
            else {
                if (*str1++ != c)
                    return false;
            }
        } while (str1_len >= 0 && c != '\0');
    }

    return true;
}

bool GsmModemUtil::strEqual(char *str1, ssize_t str1_len, const __FlashStringHelper *str2) {
    return strEqual_P(str1, str1_len, (GM_CHAR *) str2);
}
