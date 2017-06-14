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
#ifndef _GSM_MODEM_CONST_H_
#define _GSM_MODEM_CONST_H_

#if defined(ARDUINO) && ARDUINO >= 100
#  include "Arduino.h"
#else
#  include "WProgram.h"
#endif
#include "GsmModemCore.h"

#define GM_NOP            ((const char *) "")
#define GM_EOT            '\26'
#define GM_CR             '\r'
#define GM_LF             '\n'
#define GM_INPUT_PROMPT   ((const char *) "> ")
#define GM_NUMBER_LEN     20
#define GM_END_SMS        '\x1A'  // Ctrl+z
//#define GM_END_SMS        '\x1B'  // Esc

class GsmModemConst {
    public:
        static GM_PROG_CHAR       AT[];
        static GM_PROG_CHAR       ATTENTION[];

        // Standart responses
        static GM_PROG_CHAR       OK[];
        static GM_PROG_CHAR       SHUT_OK[];
        static GM_PROG_CHAR       ERROR[];
        static GM_PROG_CHAR_ARRAY STD_RESP[];
};
#endif
