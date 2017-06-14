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
#ifndef _GSM_MODEM_DRIVER_H_
#define _GSM_MODEM_DRIVER_H_

#if defined(ARDUINO) && ARDUINO >= 100
#  include "Arduino.h"
#else
#  include "WProgram.h"
#endif
#include <StateMachine.h>
#include "GsmModemConst.h"
#include "GsmModemBuffer.h"
#include "GsmModemUtil.h"
#include "GsmModemCmd.h"

#define _RESP_TIMEOUT          5000

class GsmModemDriver;

typedef void (*GsmModemUrcCallback)(GsmModemDriver *drv, char *line, size_t len, void *data);

typedef struct GsmModemUrcHandler_ GsmModemUrcHandler;
struct GsmModemUrcHandler_ {
    GM_CHAR             *pattern;
    uint8_t              matchingType;
    GsmModemUrcCallback  callback;
    void                *data;
    GsmModemUrcHandler  *next;
};

class GsmModemDriver: public GsmModemConst, public GsmModemUtil, public StateMachine, public Print {
    public:
        void execCmd(const char cmd, const char *param1 = NULL, const char *param2 = NULL, const char *param3 = NULL);
        void execCmd(const char *cmd, const char *param1 = NULL, const char *param2 = NULL, const char *param3 = NULL);
        void execCmd(GM_PCHAR *cmd, const char *param1 = NULL, const char *param2 = NULL, const char *param3 = NULL);
        void waitResponse(GM_CHAR_ARRAY *expect = STD_RESP, uint32_t timeout = 0);
        inline void waitResponse(uint32_t timeout, GM_CHAR_ARRAY *expect = STD_RESP) {
            waitResponse(expect, timeout);
        };
        void flush(void);
        size_t write(uint8_t c);
        void loop(void);
        void addUrcHandler(GM_CHAR *pattern, uint8_t matchingType, GsmModemUrcCallback callback, void *data = NULL);
        void addUrcHandler(const __FlashStringHelper *pattern, uint8_t matchingType, GsmModemUrcCallback callback, void *data = NULL) {
            addUrcHandler((GM_CHAR *) pattern, matchingType, callback, data);
        };
        void begin(void);
        inline bool isReadyForCmd(void) {
            return _cmd.isReady();
        };
        inline GM_CHAR *cmdResponse(void) {
            return _cmd.response();
        };
        inline bool isCmdComplete(void) {
            return _cmd.isComplete();
        };
        inline bool isCmdEnd(void) {
            return _cmd.isEnd();
        };
        inline bool isCmdSuccess(void) {
            return _cmd.isSuccess();
        };
        inline bool isCmdError(void) {
            return _cmd.isError();
        };
        inline bool isCmdTimeout(void) {
            return _cmd.isTimeout();
        };
        inline uint32_t getRespTimeout(void) {
            return _respTimeout;
        };
        inline void setRespTimeout(uint32_t t) {
            _respTimeout = t;
        };
        inline void endCmd(void) {
            _cmd.end();
        };
        inline void resetCmd(void) {
            _cmd.reset();
        };
        inline bool isCmdWaitForInput(void) {
            return _cmd.isWaitForInput();
        };
        bool isCmdWaitResponse(void) {
            return _cmd.isWaitResponse();
        };
        inline char *cmdTextResponse(void) {
            return _cmd.textResponse();
        };
        inline size_t cmdTextResponseLen(void) {
            return _cmd.textResponseLen();
        };
        GsmModemDriver(Stream *port, size_t bufSize) :
            StateMachine((PState) &GsmModemDriver::IDLE),
            _port(port),
            _buf(bufSize),
            _urcHandler(NULL),
            _respTimeout(_RESP_TIMEOUT)
        {
        };

        State IDLE();
        State WAIT_CMD_RESPONSE();
        State WAIT_CMD_END();

    protected:
        Stream             *_port;
        GsmModemBuffer      _buf;
        GsmModemCmd         _cmd;
        GsmModemUrcHandler *_urcHandler;
        uint32_t            _respTimeout;

        void _sendCmdParamsAndExec(const char *param1, const char *param2, const char *param3);
        void _read(void);
        void _onLineRead(char *line, size_t len);
};

#undef _RESP_TIMEOUT

#endif
