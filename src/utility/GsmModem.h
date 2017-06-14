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
#ifndef _GSM_MODEM_H_
#define _GSM_MODEM_H_

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
#include "GsmModemPowerManagement.h"
#include "GsmModemPinManagement.h"
#include "GsmModemVoiceCallService.h"
#include "GsmModemSmsService.h"
#ifdef WITH_GSM_MODEM_INFO
#include "GsmModemInfo.h"
#endif

#define _POWER_PIN             9

class GsmModem {
    public:
        inline GsmModemDriver *drv(void) { return &_drv; };
        inline GsmModemPowerManagement *pm(void) { return &_pm; };
        inline GsmModemPinManagement *pin(void) { return &_pin; };
        inline GsmModemVoiceCallService *vcs(void) { return &_vcs; };
        inline GsmModemSmsService *sms(void) { return &_sms; };
#ifdef WITH_GSM_MODEM_INFO
        inline GsmModemInfo *info(void) { return &_info; };
#endif
        void loop(void);
        void begin(void);

        GsmModem(Stream *port, size_t bufSize,
                 uint8_t     powerSwitchPin         = _POWER_PIN,
                 uint8_t     powerSwitchActiveLevel = HIGH,
                 const char *simPin               = "",
                 const char *apn                  = NULL) :
            _drv(port, bufSize),
            _pm(&_drv, powerSwitchPin, powerSwitchActiveLevel),
            _pin(&_drv, simPin),
            _vcs(&_drv),
            _sms(&_drv)
#ifdef WITH_GSM_MODEM_INFO
            ,_info(&_drv, &_pm, &_pin, &_vcs, apn)
#endif
        {
        };

    protected:
        static void _onChangePmStatus(GsmModemPowerManagement *pm, uint8_t pmStatus, void *data);
        static void _onChangeRegStatus(GsmModemPinManagement *pin, uint8_t regStatus, void *data);

        GsmModemDriver           _drv;
        GsmModemPowerManagement  _pm;
        GsmModemPinManagement    _pin;
        GsmModemVoiceCallService _vcs;
        GsmModemSmsService       _sms;
#ifdef WITH_GSM_MODEM_INFO
        GsmModemInfo             _info;
#endif
};

#undef _POWER_PIN

#endif
