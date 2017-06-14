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
#ifndef _M590_H_
#define _M590_H_

#if defined(ARDUINO) && ARDUINO >= 100
#  include "Arduino.h"
#else
#  include "WProgram.h"
#endif
#include <utility/GsmModem.h>

#define _INIT_CMD             "&FE1+CREG=1"
#define _READY_TIMEOUT        (1200 + 2000) // 2000ms wait for command ready
#define _READY_PATTERN        "RDY"

/*
 *  AT&F        - Factory reset
 *  ATE1        - enable echo
 *  AT+CLIP=1   - Request calling line identification
 *  AT+CMEE=0   - Mobile Equipment Error Code
 *  AT+CMGF=1   - set the SMS mode to text
 *  AT+CSCS="GSM" - set TE character set
 *  AT+CPMS="SM","SM","SM" - send AT command to init memory for SMS in the SIM card
 *                response: +CPMS: <usedr>,<totalr>,<usedw>,<totalw>,<useds>,<totals>
 *  AT+CPBS="SM"- select phonebook memory storage
 *  AT+CIPSHUT  - deactivate GPRS PDP context
 *  AT+CREG=1   - enable network registration unsolicited result code
 *  AT+COLP=1   - wait for connect after ATD,  AT+COLP=0 - no wait after ATD and return OK
 *  AT+CNMI=2,0 - Disable messages about new SMS from the GSM module
 *  AT+CNMI=1,1 - indication method of new SMS (+CMTI: "SM",6)
 *  AT+DDET=1   - enable the DTMF decoding functionality
 */
#define _INIT_AFTER_REG_CMD   "+CLIP=1;"                                \
                              "+CMEE=0;"                                \
                              "+CMGF=1;"                                \
                              "+CSCS=\"GSM\";"                          \
                              "+CPMS=\"SM\",\"SM\",\"SM\";+CPBS=\"SM\";"\
                              "+CIPSHUT;"                               \
                              "+CNMI=1,1;"                              \
                              "+DDET=1;"                                \
                              "+COLP=1"

class SIM900: public GsmModem {
    public:
        SIM900(Stream     *port,
               size_t      bufSize,
               uint8_t     powerSwitchPin,
               const char *simPin = "",
               const char *apn    = NULL) :
            GsmModem(port, bufSize, powerSwitchPin, HIGH, simPin, apn)
        {
            _pm.setReadyPattern(F(_READY_PATTERN));
            _pm.setReadyTimeout(_READY_TIMEOUT);
            _pm.setInitCmd(F(_INIT_CMD));
            _pin.setInitAfterRegCmd(F(_INIT_AFTER_REG_CMD));
        };
};

#undef _INIT_AFTER_REG_CMD
#undef _READY_TIMEOUT
#undef _READY_PATTERN

#endif
