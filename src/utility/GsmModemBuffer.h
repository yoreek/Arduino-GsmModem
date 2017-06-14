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
#ifndef _GSM_MODEM_BUFFER_H_
#define _GSM_MODEM_BUFFER_H_

#if defined(ARDUINO) && ARDUINO >= 100
#  include "Arduino.h"
#else
#  include "WProgram.h"
#endif
#include "GsmModemUtil.h"
#include "GsmModemConst.h"

class GsmModemBuffer {
    public:
        char *start;
        char *end;
        char *last;

        void begin(void);
        void reset(void);
        //char *nextLine(char **cur, size_t *len);
        void removeLine(char *start, char *end);
        inline size_t available(void) {
            return end - last;
        };
        inline bool isEmpty(void) {
            return last == start;
        };
        inline char *lastLine(void) {
            return _lastLine;
        };
        void put(uint8_t c);
        //uint8_t peek(int pos);

        GsmModemBuffer(size_t bufSize) :
            _bufSize(bufSize)
        {
        };

    protected:
        size_t _bufSize;
        char  *_lastLine;
};
#endif
