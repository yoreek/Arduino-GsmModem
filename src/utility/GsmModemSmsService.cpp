#include "GsmModemSmsService.h"
#ifndef WITH_GSM_MODEM_DEBUG
#  undef WITH_DEBUG
#endif
#include <DebugUtil.h>

#undef SM_CLASS
#define SM_CLASS GsmModemSmsService

#define _IS_SENDING_IN_PROGRESS()  (_phoneNumber[0] != '\0')

// Command
GM_PROG_CHAR GsmModemSmsService::CMGS[] = "+CMGS";

State GsmModemSmsService::IDLE(void) {
    if (isStateUpdated()) {
        DEBUG("idle");
    }
    if (_IS_SENDING_IN_PROGRESS() && _sendingStatus == GM_SMS_IDLE
        && (_lastRetry == 0 || (_lastRetry + _retryInterval) < millis())) {
        _lastResponse = NULL;
        _lastResult   = GM_SMS_RES_NONE;
        SM_CALL2(SEND_SMS, IDLE, IDLE);
    }
}

State GsmModemSmsService::SEND_SMS(void) {
    if (isStateUpdated()) {
        _sendingStatus = GM_SMS_SENDING_NUMBER;
    }
    if (_drv->isReadyForCmd()) {
        _drv->execCmd((GM_PCHAR *) CMGS, "=\"", _phoneNumber, "\"");
        _drv->waitResponse(_sendTimeout);
        _lastRetry = millis();
        _retries--;
        SM_GOTO(WAIT_FOR_ANSWER);
    }
}

State GsmModemSmsService::WAIT_FOR_ANSWER(void) {
    if (isStateUpdated()) {
        DEBUG("wait for answer");
        _sendingStatus = GM_SMS_SENDING_TEXT;
    }
    if (_drv->isCmdComplete()) {
        if (_drv->isCmdSuccess()) {
            _sendingStatus = GM_SMS_IDLE;
            _lastResponse  = _drv->cmdResponse();
            _lastResult    = GM_SMS_RES_SUCCESS;
            _drv->endCmd();
            GM_RESET_STR(_phoneNumber);

            SM_RETURN_SUCCESS();
        }
        else if (_drv->isCmdError()) {
            _sendingStatus = GM_SMS_IDLE;
            _lastResponse  = _drv->cmdResponse();
            _lastResult    = GM_SMS_RES_ERROR;
            _drv->endCmd();

            if (_retries == 0)
                GM_RESET_STR(_phoneNumber);

            SM_RETURN_FAIL();
        }
        else if (_drv->isCmdTimeout()) {
            _sendingStatus       = GM_SMS_IDLE;
            _lastResponse = NULL;
            _lastResult   = GM_SMS_RES_TIMEOUT;
            _drv->endCmd();

            if (_retries == 0)
                GM_RESET_STR(_phoneNumber);

            SM_RETURN_FAIL();
        }
    }
    else if (_sendingStatus == GM_SMS_SENDING_TEXT && _drv->isCmdWaitForInput()) {
        _sendMsg(this, _sendData, _phoneNumber);
        _drv->print(GM_END_SMS);
        _sendingStatus = GM_SMS_SENDING_END;
    }
}

void GsmModemSmsService::begin(void) {
    pause();
}

void GsmModemSmsService::pause(void) {
    if (isPaused())
        return;

    _sendingStatus = GM_SMS_IDLE;

    StateMachine::pause();
}

void GsmModemSmsService::restart(void) {
    if (!isPaused())
        return;

    _sendingStatus = GM_SMS_IDLE;

    StateMachine::restart();

    SM_GOTO(IDLE);
}

bool GsmModemSmsService::sendTo(const char *phoneNumber, GsmModemSmsSendMsg sendMsg,
                                void *sendData, uint8_t retries, uint32_t retryInterval)
{
    if (_IS_SENDING_IN_PROGRESS())
        return false;

    strncpy(_phoneNumber, phoneNumber, GM_NUMBER_LEN);
    _phoneNumber[GM_NUMBER_LEN] = '\0';

    _sendMsg        = sendMsg;
    _sendData       = sendData;
    _retries        = retries;
    _retryInterval  = retryInterval;
    _lastRetry      = 0;

    return true;
}
