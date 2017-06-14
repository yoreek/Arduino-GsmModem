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
#ifndef _GSM_MODEM_VOICE_CALL_SERVICE_H_
#define _GSM_MODEM_VOICE_CALL_SERVICE_H_

#if defined(ARDUINO) && ARDUINO >= 100
#  include "Arduino.h"
#else
#  include "WProgram.h"
#endif
#include "GsmModemBaseService.h"

#define _DIAL_TIMEOUT            60000
#define _CALL_RETRIES            3
#define _CALL_RETRY_INTERVAL     30000

#define GM_VCS_IDLE              0
#define GM_VCS_CALLING           1
#define GM_VCS_RECEIVING_CALL    2
#define GM_VCS_TALKING           3
#define GM_VCS_HANGING_UP        4
#define GM_VCS_ANSWERING         5

#define GM_VCS_RES_NONE          0
#define GM_VCS_RES_SUCCESS       1
#define GM_VCS_RES_ERROR         2
#define GM_VCS_RES_TIMEOUT       3

class GsmModemVoiceCallService;

typedef void (*GsmModemVoiceCallCallback)
(GsmModemVoiceCallService *vcs, uint8_t status, const char *phoneNumber, const char *response, void *data);

typedef struct GsmModemVcsHandler_ GsmModemVcsHandler;
struct GsmModemVcsHandler_ {
    GsmModemVoiceCallCallback  callback;
    void                      *data;
    GsmModemVcsHandler        *next;
};

#ifdef GSM_MODEM_DTMF_ENABLED
typedef void (*GsmModemDtmfCallback)
(GsmModemVoiceCallService *vcs, char dtmf, void *data);

typedef struct GsmModemVcsDtmfHandler_ GsmModemVcsDtmfHandler;
struct GsmModemVcsDtmfHandler_ {
    GsmModemDtmfCallback       callback;
    void                      *data;
    GsmModemVcsDtmfHandler     *next;
};
#endif

class GsmModemVoiceCallService: public GsmModemBaseService {
    public:
        // Commands
        static GM_PROG_CHAR A[];
        static GM_PROG_CHAR D[];
        static GM_PROG_CHAR H[];
        // Responses
        static GM_PROG_CHAR NO_CARRIER[];
        static GM_PROG_CHAR NO_DIALTONE[];
        static GM_PROG_CHAR NO_ANSWER[];
        static GM_PROG_CHAR BUSY[];
        static GM_PROG_CHAR_ARRAY CALL_RESP[];
        // Unsolicited Result Codes (URC)
        static GM_PROG_CHAR RING[];
        static GM_PROG_CHAR CLIP[];
#ifdef GSM_MODEM_DTMF_ENABLED
        static GM_PROG_CHAR DTMF[];
#endif

        static void ringUrcDetected(GsmModemDriver *drv, char *line, size_t len, void *data);
        static void clipUrcDetected(GsmModemDriver *drv, char *line, size_t len, void *data);
        static void noCarrierUrcDetected(GsmModemDriver *drv, char *line, size_t len, void *data);
#ifdef GSM_MODEM_DTMF_ENABLED
        static void dtmfUrcDetected(GsmModemDriver *drv, char *line, size_t len, void *data);
#endif
        bool callTo(const char *phoneNumber, uint8_t retries = _CALL_RETRIES,
                    uint32_t retryInterval = _CALL_RETRY_INTERVAL);
        void restart(void);
        void pause(void);
        void onVoiceCall(GsmModemVoiceCallCallback callback, void *data = NULL);
#ifdef GSM_MODEM_DTMF_ENABLED
        void onDtmf(GsmModemDtmfCallback callback, void *data = NULL);
#endif
        inline void hangup() { _callStatus = GM_VCS_HANGING_UP; };
        inline void answer() { _callStatus = GM_VCS_ANSWERING; };
        void begin(void);
        inline uint32_t getDialTimeout(void) {
            return _dialTimeout;
        };
        inline void setDialTimeout(uint32_t t) {
            _dialTimeout = t;
        };
        inline const char *incomingCallNumber(void) {
            return _incomingCallNumber;
        };
        inline const char *outgoingCallNumber(void) {
            return _outgoingCallNumber;
        };
        inline const char *lastCallResponse(void) {
            return _lastCallResponse;
        };
        inline uint8_t lastCallResult(void) {
            return _lastCallResult;
        };
        inline uint8_t getStatus(void) {
            return _callStatus;
        };

        GsmModemVoiceCallService(GsmModemDriver *drv) :
            GsmModemBaseService(drv),
            _callStatus(GM_VCS_IDLE),
            _incomingCallNumber(),
            _outgoingCallNumber(),
            _voiceCallHandler(NULL),
            _dialTimeout(_DIAL_TIMEOUT),
            _lastCallResponse(NULL),
            _lastCallResult(GM_VCS_RES_NONE)
        {
        };

        State IDLE();
        State CALLING();
        State WAIT_FOR_ANSWER();
        State HANGING_UP();
        State CHECK_FOR_HANG_UP();
        State ANSWERING();

    protected:
        void _processCallback(uint8_t status, const char *phoneNumber, const char *response);
#ifdef GSM_MODEM_DTMF_ENABLED
        void _processDtmfCallback(char dtmf);
#endif

        uint8_t             _callStatus;
        char                _incomingCallNumber[GM_NUMBER_LEN + 1];
        char                _outgoingCallNumber[GM_NUMBER_LEN + 1];
        uint8_t             _callRetries;
        uint32_t            _callRetryInterval;
        uint32_t            _callLastRetry;
        GsmModemVcsHandler *_voiceCallHandler;
#ifdef GSM_MODEM_DTMF_ENABLED
        GsmModemVcsDtmfHandler *_dtmfHandler;
#endif
        uint32_t            _dialTimeout;
        const char         *_lastCallResponse;
        uint8_t             _lastCallResult;
};

#undef _DIAL_TIMEOUT
#undef _CALL_RETRIES
#undef _CALL_RETRY_INTERVAL

#endif
