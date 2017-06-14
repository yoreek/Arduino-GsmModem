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
#ifndef _GSM_MODEM_SMS_SERVICE_H_
#define _GSM_MODEM_SMS_SERVICE_H_

#if defined(ARDUINO) && ARDUINO >= 100
#  include "Arduino.h"
#else
#  include "WProgram.h"
#endif
#include "GsmModemBaseService.h"

#define _SMS_RETRIES             3
#define _SMS_RETRY_INTERVAL      30000
#define _SMS_SEND_TIMEOUT        10000

#define GM_SMS_IDLE              0
#define GM_SMS_SENDING_NUMBER    1
#define GM_SMS_SENDING_TEXT      2
#define GM_SMS_SENDING_END       3

#define GM_SMS_RES_NONE          0
#define GM_SMS_RES_SUCCESS       1
#define GM_SMS_RES_ERROR         2
#define GM_SMS_RES_TIMEOUT       3

class GsmModemSmsService;

typedef void (*GsmModemSmsSendMsg)
(GsmModemSmsService *sms, void *data, const char *phoneNumber);

class GsmModemSmsService: public GsmModemBaseService {
    public:
        // Command
        static GM_PROG_CHAR CMGS[];

        void restart(void);
        void pause(void);
        void begin(void);
        inline uint32_t getSendTimeout(void) {
            return _sendTimeout;
        };
        inline void setSendTimeout(uint32_t t) {
            _sendTimeout = t;
        };
        bool sendTo(const char *phoneNumber, GsmModemSmsSendMsg sendMsg = NULL, void *data = NULL,
                    uint8_t retries = _SMS_RETRIES, uint32_t retryInterval = _SMS_RETRY_INTERVAL);

        GsmModemSmsService(GsmModemDriver *drv) :
            GsmModemBaseService(drv),
            _sendingStatus(GM_SMS_IDLE),
            _phoneNumber(),
            _sendTimeout(_SMS_SEND_TIMEOUT)
        {
        };

        State IDLE();
        State SEND_SMS();
        State WAIT_FOR_ANSWER();

    protected:
        void _processCallback(uint8_t status, const char *phoneNumber, const char *response);

        uint8_t             _sendingStatus;
        GsmModemSmsSendMsg  _sendMsg;
        void               *_sendData;
        char                _phoneNumber[GM_NUMBER_LEN + 1];
        uint8_t             _retries;
        uint32_t            _retryInterval;
        uint32_t            _lastRetry;
        const char         *_lastResponse;
        uint8_t             _lastResult;
        uint32_t            _sendTimeout;
};

#undef _SMS_RETRIES
#undef _SMS_RETRY_INTERVAL
#undef _SMS_SEND_TIMEOUT

#endif
