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
#ifndef _GSM_MODEM_CORE_H_
#define _GSM_MODEM_CORE_H_

#if defined(ARDUINO) && ARDUINO >= 100
#  include "Arduino.h"
#else
#  include "WProgram.h"
#endif
#ifndef WITH_GSM_MODEM_DEBUG
#  undef WITH_DEBUG
#endif
#include <DebugUtil.h>

#ifndef ssize_t
    typedef int ssize_t;
#endif

#define GM_CHAR            const char
#define GM_CHAR_ARRAY      const char* const
#define GM_PROG_CHAR       PROGMEM GM_CHAR
#define GM_PROG_CHAR_ARRAY PROGMEM GM_CHAR_ARRAY
#define GM_PCHAR           const __FlashStringHelper

#define GM_STRINGIFY_(x)   #x
#define GM_STRINGIFY(x)    GM_STRINGIFY_(x)
#define GM_RESET_STR(s)    do { (s)[0] = '\0'; } while (0)

#endif
