#include "GsmModemVoiceCallService.h"
#ifndef WITH_GSM_MODEM_DEBUG
#  undef WITH_DEBUG
#endif
#include <DebugUtil.h>

#undef SM_CLASS
#define SM_CLASS GsmModemVoiceCallService

// Commands
GM_PROG_CHAR GsmModemVoiceCallService::A[]                = "A";
GM_PROG_CHAR GsmModemVoiceCallService::D[]                = "D";
GM_PROG_CHAR GsmModemVoiceCallService::H[]                = "H";

// Responses
GM_PROG_CHAR GsmModemVoiceCallService::NO_CARRIER[]       = "NO CARRIER";
GM_PROG_CHAR GsmModemVoiceCallService::NO_ANSWER[]        = "NO ANSWER";
GM_PROG_CHAR GsmModemVoiceCallService::NO_DIALTONE[]      = "NO DIALTONE";
GM_PROG_CHAR GsmModemVoiceCallService::BUSY[]             = "BUSY";
GM_PROG_CHAR_ARRAY GsmModemVoiceCallService::CALL_RESP[]  = {GsmModemVoiceCallService::OK,
                                                             GsmModemVoiceCallService::ERROR,
                                                             GsmModemVoiceCallService::NO_CARRIER,
                                                             GsmModemVoiceCallService::NO_ANSWER,
                                                             GsmModemVoiceCallService::NO_DIALTONE,
                                                             GsmModemVoiceCallService::BUSY,
                                                             NULL};
// Unsolicited Result Codes (URC)
GM_PROG_CHAR GsmModemVoiceCallService::RING[]             = "RING";
GM_PROG_CHAR GsmModemVoiceCallService::CLIP[]             = "+CLIP";
#ifdef GSM_MODEM_DTMF_ENABLED
GM_PROG_CHAR GsmModemVoiceCallService::DTMF[]             = "+DTMF";
#endif

#define _IS_OUTGOING_CALL_IN_PROGRESS()                   (_outgoingCallNumber[0] != '\0')

State GsmModemVoiceCallService::IDLE(void) {
    if (_IS_OUTGOING_CALL_IN_PROGRESS() && _callStatus == GM_VCS_IDLE
        && (_callLastRetry == 0 || (_callLastRetry + _callRetryInterval) < millis())
    ) {
        _callStatus       = GM_VCS_CALLING;
        _lastCallResponse = NULL;
        _lastCallResult   = GM_VCS_RES_NONE;
        SM_CALL2(CALLING, IDLE, IDLE);
    }
    if (_callStatus == GM_VCS_HANGING_UP) {
        SM_CALL2(HANGING_UP, IDLE, IDLE);
    }
    if (_callStatus == GM_VCS_ANSWERING) {
        SM_CALL2(ANSWERING, IDLE, IDLE);
    }
}

State GsmModemVoiceCallService::CALLING(void) {
    if (isStateUpdated()) {
        DEBUG("call to: %s", _outgoingCallNumber);
        _processCallback(_callStatus, _outgoingCallNumber, _drv->cmdResponse());
    }
    if (_drv->isReadyForCmd()) {
        _drv->execCmd((GM_PCHAR *) D, _outgoingCallNumber, ";");
        _drv->waitResponse(CALL_RESP, _dialTimeout);
        _callLastRetry = millis();
        _callRetries--;
        SM_GOTO(WAIT_FOR_ANSWER);
    }
}

State GsmModemVoiceCallService::WAIT_FOR_ANSWER(void) {
    if (isStateUpdated()) {
        DEBUG("wait for answer");
    }
    if (_drv->isCmdComplete()) {
        if (_drv->isCmdSuccess()) {
            _callStatus       = GM_VCS_TALKING;
            _lastCallResponse = _drv->cmdResponse();
            _lastCallResult   = GM_VCS_RES_SUCCESS;
            _drv->endCmd();
            _processCallback(_callStatus, _outgoingCallNumber, _lastCallResponse);
            GM_RESET_STR(_outgoingCallNumber);

            SM_RETURN_SUCCESS();
        }
        else if (_drv->isCmdError()) {
            _callStatus       = GM_VCS_IDLE;
            _lastCallResponse = _drv->cmdResponse();
            _lastCallResult   = GM_VCS_RES_ERROR;
            _processCallback(_callStatus, _outgoingCallNumber, _lastCallResponse);
            _drv->endCmd();

            if (_callRetries == 0)
                GM_RESET_STR(_outgoingCallNumber);

            SM_RETURN_FAIL();
        }
        else if (_drv->isCmdTimeout()) {
            _callStatus       = GM_VCS_HANGING_UP;
            _lastCallResponse = NULL;
            _lastCallResult   = GM_VCS_RES_TIMEOUT;
            _drv->endCmd();

            if (_callRetries == 0)
                GM_RESET_STR(_outgoingCallNumber);

            SM_RETURN_FAIL();
        }
    }
}

State GsmModemVoiceCallService::HANGING_UP(void) {
    if (isStateUpdated()) {
        DEBUG("Hanging up");
        _processCallback(_callStatus, _outgoingCallNumber, _drv->cmdResponse());
    }
    if (_drv->isReadyForCmd()) {
        _drv->execCmd((GM_PCHAR *) H);
        _drv->waitResponse();
        SM_GOTO(CHECK_FOR_HANG_UP);
    }
}

State GsmModemVoiceCallService::CHECK_FOR_HANG_UP(void) {
    if (isStateUpdated()) {
        DEBUG("check for hang up");
    }
    if (_drv->isCmdComplete()) {
        if (_drv->isCmdSuccess()) {
            _callStatus = GM_VCS_IDLE;
            GM_RESET_STR(_incomingCallNumber);
            _drv->endCmd();
            _processCallback(_callStatus, _outgoingCallNumber, _drv->cmdResponse());
            SM_RETURN_SUCCESS();
        }
        else {
            _callStatus = GM_VCS_IDLE;
            _drv->endCmd();
            SM_RETURN_FAIL();
        }
    }
}

State GsmModemVoiceCallService::ANSWERING(void) {
    if (isStateUpdated()) {
        DEBUG("Answering");
        _processCallback(_callStatus, _outgoingCallNumber, _drv->cmdResponse());
    }
    if (_drv->isReadyForCmd()) {
        _drv->execCmd((GM_PCHAR *) A);
        _drv->waitResponse();
        SM_GOTO(WAIT_FOR_ANSWER);
    }
}

void GsmModemVoiceCallService::ringUrcDetected(GsmModemDriver *drv, char *line, size_t len, void *data) {
    GsmModemVoiceCallService *self = (GsmModemVoiceCallService *) data;

    self->_callStatus = GM_VCS_RECEIVING_CALL;
    GM_RESET_STR(self->_incomingCallNumber);
}

void GsmModemVoiceCallService::noCarrierUrcDetected(GsmModemDriver *drv, char *line, size_t len, void *data) {
    GsmModemVoiceCallService *self = (GsmModemVoiceCallService *) data;

    self->_callStatus = GM_VCS_IDLE;
    GM_RESET_STR(self->_incomingCallNumber);

    self->_processCallback(self->_callStatus, NULL, NULL);
}

void GsmModemVoiceCallService::clipUrcDetected(GsmModemDriver *drv, char *line, size_t len, void *data) {
    GsmModemVoiceCallService *self = (GsmModemVoiceCallService *) data;

    line += sizeof("+CLIP: ") - 1;
    len  -= sizeof("+CLIP: ") - 1;

    //DEBUG("clip: [%*s]", len, line);
    if (sscanf(line, "\"%" GM_STRINGIFY(GM_NUMBER_LEN) "[^\"]\"", self->_incomingCallNumber) != 1) {
        GM_RESET_STR(self->_incomingCallNumber);
    }

    DEBUG("number: %s", self->_incomingCallNumber);

    self->_callStatus = GM_VCS_RECEIVING_CALL;

    self->_processCallback(self->_callStatus, self->_incomingCallNumber, NULL);
}

#ifdef GSM_MODEM_DTMF_ENABLED
void GsmModemVoiceCallService::dtmfUrcDetected(GsmModemDriver *drv, char *line, size_t len, void *data) {
    GsmModemVoiceCallService *self = (GsmModemVoiceCallService *) data;

    if (len >= (sizeof("+DTMF:1") - 1)) {
        char dtmf = line[sizeof("+DTMF:1") - 2];
        DEBUG("dtmf: [%c]", dtmf);
        self->_processDtmfCallback(dtmf);
    }
    else {
        DEBUG("Invalid DTMF: [%*s]", len, line);
    }
}
#endif

bool GsmModemVoiceCallService::callTo(const char *phoneNumber, uint8_t retries,
                                      uint32_t retryInterval)
{
    if (_IS_OUTGOING_CALL_IN_PROGRESS())
        return false;

    strncpy(_outgoingCallNumber, phoneNumber, GM_NUMBER_LEN);
    _outgoingCallNumber[GM_NUMBER_LEN] = '\0';

    _callRetries        = retries;
    _callRetryInterval  = retryInterval;
    _callLastRetry      = 0;

    return true;
}

void GsmModemVoiceCallService::pause(void) {
    if (isPaused())
        return;

    _callStatus = GM_VCS_IDLE;

    StateMachine::pause();
}

void GsmModemVoiceCallService::restart(void) {
    if (!isPaused())
        return;

    _callStatus = GM_VCS_IDLE;

    StateMachine::restart();

    SM_GOTO(IDLE);
}

void GsmModemVoiceCallService::onVoiceCall(GsmModemVoiceCallCallback callback, void *data) {
    GsmModemVcsHandler *h = (GsmModemVcsHandler *) calloc(1, sizeof(GsmModemVcsHandler));
    if (h == NULL) {
        while (true)
            ;
    }

    h->callback = callback;
    h->data     = data;

    h->next     = _voiceCallHandler;
    _voiceCallHandler = h;
}

#ifdef GSM_MODEM_DTMF_ENABLED
void GsmModemVoiceCallService::onDtmf(GsmModemDtmfCallback callback, void *data) {
    GsmModemVcsDtmfHandler *h = (GsmModemVcsDtmfHandler *) calloc(1, sizeof(GsmModemVcsDtmfHandler));
    if (h == NULL) {
        while (true)
            ;
    }

    h->callback = callback;
    h->data     = data;

    h->next     = _dtmfHandler;
    _dtmfHandler = h;
}
#endif

void GsmModemVoiceCallService::_processCallback(uint8_t status, const char *phoneNumber, const char *response) {
    GsmModemVcsHandler *h;

    for (h = _voiceCallHandler; h != NULL; h = h->next) {
        h->callback(this, status, phoneNumber, response, h->data);
    }
}

#ifdef GSM_MODEM_DTMF_ENABLED
void GsmModemVoiceCallService::_processDtmfCallback(char dtmf) {
    GsmModemVcsDtmfHandler *h;

    for (h = _dtmfHandler; h != NULL; h = h->next) {
        h->callback(this, dtmf, h->data);
    }
}
#endif

void GsmModemVoiceCallService::begin(void) {
    _drv->addUrcHandler(RING, GM_MATCH_EQUALS, ringUrcDetected, this);
    _drv->addUrcHandler(CLIP, GM_MATCH_STARTS_WITH, clipUrcDetected, this);
    _drv->addUrcHandler(NO_CARRIER, GM_MATCH_EQUALS, noCarrierUrcDetected, this);
#ifdef GSM_MODEM_DTMF_ENABLED
    _drv->addUrcHandler(DTMF, GM_MATCH_STARTS_WITH, dtmfUrcDetected, this);
#endif
    pause();
}
