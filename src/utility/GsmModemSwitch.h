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
#ifndef _GSM_MODEM_SWITCH_H_
#define _GSM_MODEM_SWITCH_H_

#if defined(ARDUINO) && ARDUINO >= 100
#  include "Arduino.h"
#else
#  include "WProgram.h"
#endif

class GsmModemSwitch {
    public:
        GsmModemSwitch(uint8_t pin, uint8_t activeLevel = HIGH) :
            _pin(pin),
            _activeLevel(activeLevel)
        {
        };
        void begin(void) {
            turnOff();
            pinMode(_pin, OUTPUT);
        };
        inline void turnOn(void) {
            _switch(_activeLevel);
        };
        inline void turnOff(void) {
            _switch(_activeLevel == HIGH ? LOW : HIGH);
        };
        inline bool isTurnedOn(void) {
            return digitalRead(_pin) == _activeLevel ? true : false;
        };
    private:
        uint8_t _pin;
        uint8_t _activeLevel;
        void _switch(uint8_t level) {
            digitalWrite(_pin, level);
        };
};

#endif
