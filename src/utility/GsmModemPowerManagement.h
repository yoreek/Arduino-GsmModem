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
#ifndef _GSM_MODEM_POWER_MANAGEMENT_H_
#define _GSM_MODEM_POWER_MANAGEMENT_H_

#if defined(ARDUINO) && ARDUINO >= 100
#  include "Arduino.h"
#else
#  include "WProgram.h"
#endif
#include "GsmModemBaseService.h"
#include "GsmModemSwitch.h"

#define _POWER_PULSE_WIDTH     600
#define _READY_TIMEOUT         (1200 + 2000) // 2000ms wait for command ready
#define _CHECK_ALIVE_INTERVAL  30000

#define GM_PM_NONE               0
#define GM_PM_PWR_STATE          (1<<0)
#define GM_PM_READY              (1<<1)
#define GM_PM_INITIALIZED        (1<<2)

class GsmModemPowerManagement;

typedef void (*GsmModemPmCallback)(GsmModemPowerManagement *pm, uint8_t status, void *data);

typedef struct GsmModemPmHandler_ GsmModemPmHandler;
struct GsmModemPmHandler_ {
    GsmModemPmCallback   callback;
    void                *data;
    GsmModemPmHandler   *next;
};

class GsmModemPowerManagement: public GsmModemBaseService {
    public:
        GsmModemPowerManagement(GsmModemDriver *drv,
                                uint8_t powerSwitchPin,
                                uint8_t powerSwitchActiveLevel) :
            GsmModemBaseService(drv),
            _powerSwitch(powerSwitchPin, powerSwitchActiveLevel),
            _status(GM_PM_NONE),
            _nextCheck(0),
            _changeStatusHandler(NULL),
            _powerPulseWidth(_POWER_PULSE_WIDTH),
            _readyTimeout(_READY_TIMEOUT),
            _checkAliveInterval(_CHECK_ALIVE_INTERVAL),
            _initCmd(NULL),
            _readyPattern(NULL)
        {
        };

        static void readyUrcDetected(GsmModemDriver *drv, char *line, size_t len, void *data);
        void begin(void);
        void onChangeStatus(GsmModemPmCallback callback, void *data = NULL);
        uint8_t getStatus(void) {
            return _status;
        };
        void setStatus(uint8_t status);
        inline void setPowerPulseWitdh(uint32_t t) {
            _powerPulseWidth = t;
        };
        inline uint32_t getPowerPulseWidth(void) {
            return _powerPulseWidth;
        };
        inline void setReadyTimeout(uint32_t t) {
            _readyTimeout = t;
        };
        inline uint32_t getReadyTimeout(void) {
            return _readyTimeout;
        };
        inline void setCheckAliveInterval(uint32_t t) {
            _checkAliveInterval = t;
        };
        inline uint32_t getCheckAliveInterval(void) {
            return _checkAliveInterval;
        };
        inline GM_PCHAR *getInitCmd(void) {
            return _initCmd;
        };
        inline void setInitCmd(GM_PCHAR *cmd) {
            _initCmd = cmd;
        };
        inline GM_PCHAR *getReadyPattern(void) {
            return _readyPattern;
        };
        inline void setReadyPattern(GM_PCHAR *pattern) {
            _readyPattern = pattern;
        };

        State IDLE();
        State START_CHECK_FOR_ALIVE();
        State CHECK_FOR_ALIVE();
        State START_INIT();
        State WAIT_FOR_INIT();
        State TOGGLE_POWER();
        State TOGGLE_POWER2();
        State WAIT_FOR_READY();

    protected:
        GsmModemSwitch     _powerSwitch;
        uint8_t            _status;
        uint32_t           _nextCheck;
        GsmModemPmHandler *_changeStatusHandler;
        uint32_t           _powerPulseWidth;
        uint32_t           _readyTimeout;
        uint32_t           _checkAliveInterval;
        GM_PCHAR          *_initCmd;
        GM_PCHAR          *_readyPattern;

        void _processChangeStatusCallback(void);
};

#undef _POWER_PULSE_WIDTH
#undef _READY_TIMEOUT
#undef _CHECK_ALIVE_INTERVAL

#endif
