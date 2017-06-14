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
#ifndef _GSM_MODEM_BASE_SERVICE_H_
#define _GSM_MODEM_BASE_SERVICE_H_

#if defined(ARDUINO) && ARDUINO >= 100
#  include "Arduino.h"
#else
#  include "WProgram.h"
#endif
#include <StateMachine.h>
#include "GsmModemCore.h"
#include "GsmModemConst.h"
#include "GsmModemUtil.h"
#include "GsmModemDriver.h"

class GsmModemBaseService: public GsmModemConst, public GsmModemUtil, public StateMachine, public Print {
    public:
        GsmModemBaseService(GsmModemDriver *drv) :
            StateMachine((PState) &GsmModemBaseService::IDLE),
            _drv(drv)
        {
        };

        size_t write(uint8_t c);
        virtual void begin(void) = 0;
        virtual State IDLE() = 0;

    protected:
        GsmModemDriver     *_drv;
};

#endif
