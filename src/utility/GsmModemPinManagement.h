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
#ifndef _GSM_MODEM_PIN_MANAGEMENT_H_
#define _GSM_MODEM_PIN_MANAGEMENT_H_

#if defined(ARDUINO) && ARDUINO >= 100
#  include "Arduino.h"
#else
#  include "WProgram.h"
#endif
#include "GsmModemBaseService.h"

#define _CHECK_PIN_INTERVAL    30000
#define _CHECK_REG_INTERVAL    30000

#define GM_PIN_MIN_REMAINING_ATTEMPTS 1

#define GM_REG_NONE                   0
#define GM_REG_REGISTERED             1
#define GM_REG_SEARCHING              2
#define GM_REG_DENIED                 3
#define GM_REG_UNKNOWN                4
#define GM_REG_ROAMING                5

#define GM_PIN_UNKNOWN                0
#define GM_PIN_READY                  1
#define GM_PIN_NOT_READY              2
#define GM_PIN_NOT_INSERTED           3
#define GM_PIN_PH_SIM_PIN             4
#define GM_PIN_PH_SIM_PUK             5
#define GM_PIN_SIM_PIN                6
#define GM_PIN_SIM_PUK                7
#define GM_PIN_SIM_PIN2               8
#define GM_PIN_SIM_PUK2               9

class GsmModemPinManagement;

typedef void (*GsmModemPinCallback)(GsmModemPinManagement *pin, uint8_t status, void *data);

typedef struct GsmModemPinHandler_ GsmModemPinHandler;
struct GsmModemPinHandler_ {
    GsmModemPinCallback  callback;
    void                *data;
    GsmModemPinHandler  *next;
};

struct GsmModemInputPinTimesRemained {
    uint8_t pin1, pin2, puk1, puk2;
};

class GsmModemPinManagement: public GsmModemBaseService {
    public:
        GsmModemPinManagement(GsmModemDriver *drv, const char *simPin = "") :
            GsmModemBaseService(drv),
            _simPin(simPin),
            _pinStatus(GM_PIN_UNKNOWN),
            _regStatus(GM_REG_NONE),
            _initializedAfterReg(false),
            _nextPinCheck(0),
            _nextRegCheck(0),
            _inputPinTimesRemained({0,0,0,0}),
            _initAfterRegCmd(NULL),
            _checkPinInterval(_CHECK_PIN_INTERVAL),
            _checkRegInterval(_CHECK_REG_INTERVAL),
            _changePinStatusHandler(NULL),
            _changeRegStatusHandler(NULL)
        {
        };

        // Command
        static GM_PROG_CHAR CPIN[];
        static GM_PROG_CHAR CREG[];
        static GM_PROG_CHAR SPIC[];

        // Unsolicited Result Codes (URC)
        static GM_PROG_CHAR CALL_READY[];

        // Responses
        static GM_PROG_CHAR READY[];
        static GM_PROG_CHAR NOT_READY[];
        static GM_PROG_CHAR NOT_INSERTED[];
        static GM_PROG_CHAR SIM_PIN[];
        static GM_PROG_CHAR SIM_PUK[];
        static GM_PROG_CHAR PH_SIM_PIN[];
        static GM_PROG_CHAR PH_SIM_PUK[];
        static GM_PROG_CHAR SIM_PIN2[];
        static GM_PROG_CHAR SIM_PUK2[];
        static GM_PROG_CHAR_ARRAY PIN_RESP[];

        inline uint8_t pinStatus(void) {
            return _pinStatus;
        };
        static void pinUrcDetected(GsmModemDriver *drv, char *line, size_t len, void *data);
        static void regUrcDetected(GsmModemDriver *drv, char *line, size_t len, void *data);
        static void callReadyUrcDetected(GsmModemDriver *drv, char *line, size_t len, void *data);
        void restart(void);
        void pause(void);
        void begin(void);
        inline GM_PCHAR *getInitAfterRegCmd(void) {
            return _initAfterRegCmd;
        };
        inline void setInitAfterRegCmd(GM_PCHAR *cmd) {
            _initAfterRegCmd = cmd;
        };
        inline uint32_t getCheckPinInterval(void) {
            return _checkPinInterval;
        };
        inline void setCheckPinInterval(uint32_t t) {
            _checkPinInterval = t;
        };
        inline uint32_t getCheckRegInterval(void) {
            return _checkRegInterval;
        };
        inline void setCheckRegInterval(uint32_t t) {
            _checkRegInterval = t;
        };
        inline GsmModemInputPinTimesRemained *getInputPinTimesRemained(void) {
            return &_inputPinTimesRemained;
        };
        void onChangePinStatus(GsmModemPinCallback callback, void *data = NULL);
        void onChangeRegStatus(GsmModemPinCallback callback, void *data = NULL);
        uint8_t getPinStatus(void) {
            return _pinStatus;
        };
        void setPinStatus(uint8_t pinStatus);
        uint8_t getRegStatus(void) {
            return _regStatus;
        };
        void setRegStatus(uint8_t regStatus);

        State IDLE();
        State START_CHECK_INPUT_PIN_REMANING_ATTEMPTS();
        State CHECK_INPUT_PIN_REMAINING_ATTEMPTS();
        State START_CHECK_PIN_STATUS();
        State CHECK_PIN_STATUS();
        State START_INPUT_PIN();
        State CHECK_INPUT_PIN();
        State START_CHECK_REGISTRATION();
        State CHECK_REGISTRATION();
        State START_INIT();
        State WAIT_FOR_INIT();

    protected:
        uint8_t _parseInputPinResponse(char *str, size_t len);
        uint8_t _parseRegistrationResponse(char *str, size_t len);
        uint8_t _parseUrcRegistrationResponse(char *str, size_t len);
        void _parseRemainingAttemptsResponse(char *str, size_t len);

        const char                   *_simPin;
        uint8_t                       _pinStatus;
        uint8_t                       _regStatus;
        bool                          _initializedAfterReg;
        uint32_t                      _nextPinCheck;
        uint32_t                      _nextRegCheck;
        GsmModemInputPinTimesRemained _inputPinTimesRemained;
        GM_PCHAR                     *_initAfterRegCmd;
        uint32_t                      _checkPinInterval;
        uint32_t                      _checkRegInterval;
        GsmModemPinHandler           *_changePinStatusHandler;
        GsmModemPinHandler           *_changeRegStatusHandler;
};

#undef _CHECK_PIN_INTERVAL
#undef _CHECK_REG_INTERVAL

#endif
