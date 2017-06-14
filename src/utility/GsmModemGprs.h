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
#ifndef _GSM_MODEM_GPRS_H_
#define _GSM_MODEM_GPRS_H_

#if defined(ARDUINO) && ARDUINO >= 100
#  include "Arduino.h"
#else
#  include "WProgram.h"
#endif
#include "GsmModemBaseService.h"

class GsmModemGprs: public GsmModemBaseService {
    public:
        // Command
        static GM_PROG_CHAR CGATT[];
        static GM_PROG_CHAR SAPBR[];

        GsmModemGprs(GsmModemDriver *drv,
                     const char *apn,
                     const char *user     = NULL,
                     const char *password = NULL) :
            GsmModemBaseService(drv),
            _apn(apn),
            _user(user),
            _password(password)
        {
        };

        State GPRS_CHECK(void);
        State GPRS_ATTACH(void);
        State GPRS_OPEN(void);
        State GPRS_DETACH(void);
        State GPRS_SET_CONTYPE(void);
        State GPRS_SET_APN(void);
        State GPRS_SET_USER(void);
        State GPRS_SET_PWD(void);
        State GPRS_SUCCESS(void);
        State GPRS_FAIL(void);

    protected:
        const char *_apn;
        const char *_user;
        const char *_password;
};

#endif
