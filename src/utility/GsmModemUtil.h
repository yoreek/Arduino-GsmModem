/* Arduino GsmModem Library
 * Copyright (C) 2016 by Yuriy Ustushenko
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino SdFat Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#ifndef _GSM_MODEM_UTIL_H_
#define _GSM_MODEM_UTIL_H_

#if defined(ARDUINO) && ARDUINO >= 100
#  include "Arduino.h"
#else
#  include "WProgram.h"
#endif
#include "GsmModemCore.h"
#include "GsmModemBuffer.h"
#ifndef WITH_GSM_MODEM_DEBUG
#  undef WITH_DEBUG
#endif
#include <DebugUtil.h>

#define GM_MATCH_EQUALS          0
#define GM_MATCH_CONTAINS        1
#define GM_MATCH_STARTS_WITH     2

class GsmModemUtil {
    public:
        static char *match(const char *pattern, size_t pattern_len, char *str, size_t str_len, uint8_t matchingType = GM_MATCH_EQUALS);

        static GM_CHAR *strMatch(char *str1, size_t str1_len, GM_CHAR_ARRAY *patterns, uint8_t matchingType = GM_MATCH_EQUALS);
        static GM_CHAR *strMatch_P(char *str1, size_t str1_len, GM_CHAR_ARRAY *patterns, uint8_t matchingType = GM_MATCH_EQUALS);

        static bool strMatch(char *str1, size_t str1_len, GM_CHAR *str2, uint8_t matchingType = GM_MATCH_EQUALS);
        static bool strMatch_P(char *str1, size_t str1_len, GM_CHAR *str2, uint8_t matchingType = GM_MATCH_EQUALS);
        static bool strMatch(char *str1, size_t str1_len, const __FlashStringHelper *str2, uint8_t matchingType = GM_MATCH_EQUALS);

        static bool strStartWith(char *str, ssize_t str_len, const char *substr);
        static bool strStartWith_P(char *str, ssize_t str_len, GM_CHAR *substr);
        static bool strStartWith(char *str, ssize_t str_len, const __FlashStringHelper *substr);

        static bool strEqual(char *str1, ssize_t str1_len, const char *str2);
        static bool strEqual_P(char *str1, ssize_t str1_len, GM_CHAR *str2);
        static bool strEqual(char *str1, ssize_t str1_len, const __FlashStringHelper *str2);
};
#endif
