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
#ifndef _GSM_MODEM_CMD_H_
#define _GSM_MODEM_CMD_H_

#if defined(ARDUINO) && ARDUINO >= 100
#  include "Arduino.h"
#else
#  include "WProgram.h"
#endif
#include "GsmModemConst.h"

#define GM_CMD_READY             0
#define GM_CMD_WAIT_RESPONSE     1
#define GM_CMD_BEGIN_RESPONSE    2
#define GM_CMD_COMPLETE          3
#define GM_CMD_END               4

class GsmModemCmd: public GsmModemConst {
    public:
        GsmModemCmd(void) :
            _status(GM_CMD_READY),
            _isWaitForInput(false)
        {
        };
        void begin(void);
        inline GM_CHAR *response(void) {
            return _response;
        };
        inline char *textResponse(void) {
            return _textResponse;
        };
        inline size_t textResponseLen(void) {
            return _textResponseLen;
        };
        inline uint32_t responseTimeout(void) {
            return _responseTimeout;
        };
        inline const char* const *expect(void) {
            return _expect;
        };
        bool isReady(void) {
            return _status == GM_CMD_READY;
        };
        bool isWaitResponse(void) {
            return _status == GM_CMD_WAIT_RESPONSE;
        };
        bool isBeginResponse(void) {
            return _status == GM_CMD_BEGIN_RESPONSE;
        };
        bool isComplete(void) {
            return _status == GM_CMD_COMPLETE;
        };
        bool isEnd(void) {
            return _status == GM_CMD_END;
        };
        bool isSuccess(void) {
            return _response == OK || _response == SHUT_OK;
        };
        bool isError(void) {
            return _response != NULL && _response != OK;
        };
        bool isTimeout(void) {
            return _response == NULL;
        };
        inline uint8_t status(void) {
            return _status;
        };
        inline void waitForInput(void) {
            _isWaitForInput = true;
        };
        inline bool isWaitForInput(void) {
            return _isWaitForInput;
        };
        void reset(void);
        void beginResponse(char *response);
        void waitResponse(const char* const  *expect, uint32_t timeout);
        void complete(PGM_P response, char *endResponse);
        void completeWithTimeout(void) {
            complete(NULL, NULL);
        };
        void end(void) {
            _status = GM_CMD_END;
        };

    private:
        uint8_t             _status;
        const char* const  *_expect;
        GM_CHAR            *_response;
        uint32_t            _responseTimeout;
        char               *_textResponse;
        size_t              _textResponseLen;
        bool                _isWaitForInput;
};
#endif
