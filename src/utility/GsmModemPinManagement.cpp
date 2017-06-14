#include "GsmModemPinManagement.h"
#ifndef WITH_GSM_MODEM_DEBUG
#  undef WITH_DEBUG
#endif
#include <DebugUtil.h>

#undef SM_CLASS
#define SM_CLASS GsmModemPinManagement

// Command
GM_PROG_CHAR GsmModemPinManagement::CPIN[]             = "+CPIN";
GM_PROG_CHAR GsmModemPinManagement::SPIC[]             = "+SPIC";
GM_PROG_CHAR GsmModemPinManagement::CREG[]             = "+CREG";

// Unsolicited Result Codes (URC)
GM_PROG_CHAR GsmModemPinManagement::CALL_READY[]       = "Call Ready";

// Responses
GM_PROG_CHAR GsmModemPinManagement::READY[]            = "READY";
GM_PROG_CHAR GsmModemPinManagement::NOT_READY[]        = "NOT READY";
GM_PROG_CHAR GsmModemPinManagement::NOT_INSERTED[]     = "NOT INSERTED";
GM_PROG_CHAR GsmModemPinManagement::SIM_PIN[]          = "SIM PIN";
GM_PROG_CHAR GsmModemPinManagement::SIM_PUK[]          = "SIM PUK";
GM_PROG_CHAR GsmModemPinManagement::PH_SIM_PIN[]       = "PH_SIM PIN";
GM_PROG_CHAR GsmModemPinManagement::PH_SIM_PUK[]       = "PH_SIM PUK";
GM_PROG_CHAR GsmModemPinManagement::SIM_PIN2[]         = "SIM PIN2";
GM_PROG_CHAR GsmModemPinManagement::SIM_PUK2[]         = "SIM PUK2";
GM_PROG_CHAR_ARRAY GsmModemPinManagement::PIN_RESP[]   = {GsmModemPinManagement::READY,
                                                         GsmModemPinManagement::NOT_READY,
                                                         GsmModemPinManagement::NOT_INSERTED,
                                                         GsmModemPinManagement::SIM_PIN,
                                                         GsmModemPinManagement::SIM_PUK,
                                                         GsmModemPinManagement::PH_SIM_PIN,
                                                         GsmModemPinManagement::PH_SIM_PUK,
                                                         GsmModemPinManagement::SIM_PIN2,
                                                         GsmModemPinManagement::SIM_PUK2,
                                                         NULL};

State GsmModemPinManagement::IDLE(void) {
    if (millis() > _nextPinCheck) {
        DEBUG("start check PIN");
        SM_GOTO(START_CHECK_PIN_STATUS);
    }
    if (_pinStatus == GM_PIN_READY && millis() > _nextRegCheck) {
        DEBUG("start check registration");
        SM_GOTO(START_CHECK_REGISTRATION);
    }
    if (!_initializedAfterReg && (_regStatus == GM_REG_REGISTERED || _regStatus == GM_REG_ROAMING)) {
        DEBUG("start initializtion");
        SM_GOTO(START_INIT);
    }
}

State GsmModemPinManagement::START_CHECK_PIN_STATUS(void) {
    if (isStateUpdated()) {
        DEBUG("start check PIN status");
    }
    if (_drv->isReadyForCmd()) {
        _nextPinCheck = millis() + _checkPinInterval;
        _drv->execCmd((GM_PCHAR *) CPIN, "?");
        _drv->waitResponse();
        SM_GOTO(CHECK_PIN_STATUS);
    }
}

State GsmModemPinManagement::CHECK_PIN_STATUS(void) {
    if (isStateUpdated()) {
        DEBUG("check PIN");
    }
    if (_drv->isCmdComplete()) {
        if (_drv->isCmdSuccess()) {
            setPinStatus(_parseInputPinResponse(_drv->cmdTextResponse(), _drv->cmdTextResponseLen() - 2));
            DEBUG("Response: %#*s status: %d", _drv->cmdTextResponseLen(), _drv->cmdTextResponse(), _pinStatus);
            _drv->endCmd();

            if (_pinStatus == GM_PIN_SIM_PIN) {
                DEBUG("Require PIN");
                setRegStatus(GM_REG_NONE);
                _initializedAfterReg = false;
                SM_CALL2(START_CHECK_INPUT_PIN_REMANING_ATTEMPTS, START_INPUT_PIN, IDLE);
            }
            else if (_pinStatus == GM_PIN_READY) {
                _nextRegCheck = 0;
                SM_GOTO(IDLE);
            }
            else {
                setRegStatus(GM_REG_NONE);
                _initializedAfterReg = false;
                SM_GOTO(IDLE);
            }
        }
        else {
            DEBUG("Command is failed");
            setPinStatus(GM_PIN_UNKNOWN);
            setRegStatus(GM_REG_NONE);
            _initializedAfterReg = false;
            _drv->endCmd();
            SM_GOTO(IDLE);
        }
    }
}

State GsmModemPinManagement::START_CHECK_INPUT_PIN_REMANING_ATTEMPTS(void) {
    if (isStateUpdated()) {
        DEBUG("start");
        _inputPinTimesRemained = {0, 0, 0, 0};
    }
    if (_drv->isReadyForCmd()) {
        _drv->execCmd((GM_PCHAR *) SPIC);
        _drv->waitResponse();
        SM_GOTO(CHECK_INPUT_PIN_REMAINING_ATTEMPTS);
    }
}

State GsmModemPinManagement::CHECK_INPUT_PIN_REMAINING_ATTEMPTS(void) {
    if (isStateUpdated()) {
        DEBUG("check times remained");
    }
    if (_drv->isCmdComplete()) {
        if (_drv->isCmdSuccess()) {
            _parseRemainingAttemptsResponse(_drv->cmdTextResponse(), _drv->cmdTextResponseLen() - 2);
            _drv->endCmd();
            SM_RETURN_SUCCESS();
        }
        else {
            DEBUG("Command is failed");
            _drv->endCmd();
            SM_RETURN_FAIL();
        }
    }
}

State GsmModemPinManagement::START_INPUT_PIN(void) {
    if (isStateUpdated()) {
        DEBUG("Start input PIN");
    }
    if (_inputPinTimesRemained.pin1 <= GM_PIN_MIN_REMAINING_ATTEMPTS) {
        DEBUG("PIN remaining attempts: %d", _inputPinTimesRemained.pin1);
        SM_GOTO(IDLE);
    }
    if (_drv->isReadyForCmd()) {
        DEBUG("Start input PIN: '%s'", _simPin);
        _drv->execCmd((GM_PCHAR *) CPIN, "=", _simPin);
        _drv->waitResponse();
        SM_GOTO(CHECK_INPUT_PIN);
    }
}

State GsmModemPinManagement::CHECK_INPUT_PIN(void) {
    if (isStateUpdated()) {
        DEBUG("Check input PIN");
    }
    if (_drv->isCmdComplete()) {
        if (_drv->isCmdSuccess()) {
            DEBUG("Pin '%s' is accepted", _simPin);
            _drv->endCmd();
            SM_GOTO(START_CHECK_PIN_STATUS);
        }
        else if (_drv->isCmdError()) {
            DEBUG("Pin '%s' is wrong", _simPin);
            _drv->endCmd();
            SM_GOTO(IDLE);
        }
        else {
            DEBUG("Got timeout");
            _drv->endCmd();
            SM_GOTO(START_INPUT_PIN);
        }
    }
}

#if (1)
uint8_t GsmModemPinManagement::_parseInputPinResponse(char *str, size_t len) {
    if (strStartWith_P(str, len, CPIN)) {
        str += sizeof("+CPIN: ") - 1;
        len -= sizeof("+CPIN: ") - 1;

        GM_CHAR *resp = strMatch_P(str, len, PIN_RESP);
        if (resp == READY) {
            return GM_PIN_READY;
        }
        else if (resp == NOT_READY) {
            return GM_PIN_NOT_READY;
        }
        else if (resp == NOT_INSERTED) {
            return GM_PIN_NOT_INSERTED;
        }
        else if (resp == SIM_PIN) {
            return GM_PIN_SIM_PIN;
        }
        else if (resp == SIM_PUK) {
            return GM_PIN_SIM_PUK;
        }
        else if (resp == PH_SIM_PIN) {
            return GM_PIN_PH_SIM_PIN;
        }
        else if (resp == PH_SIM_PUK) {
            return GM_PIN_PH_SIM_PUK;
        }
        else if (resp == SIM_PIN2) {
            return GM_PIN_SIM_PIN2;
        }
        else if (resp == SIM_PUK2) {
            return GM_PIN_SIM_PUK2;
        }
    }

    return GM_PIN_UNKNOWN;
}
#else
uint8_t GsmModemPinManagement::_parseInputPinResponse(char *str, size_t len) {
    if (strStartWith_P(str, len, CPIN)) {
        str += sizeof("+CPIN: ") - 1;
        len -= sizeof("+CPIN: ") - 1;

        GM_CHAR *resp = strMatch_P(str, len, PIN_RESP);
        return resp - PIN_RESP[0] + 1;
    }

    return GM_PIN_UNKNOWN;
}
#endif

void GsmModemPinManagement::_parseRemainingAttemptsResponse(char *str, size_t len) {
    int pin1, pin2, puk1, puk2;
    if (strStartWith_P(str, len, SPIC)) {
        str += sizeof("+SPIC: ") - 1;
        len -= sizeof("+SPIC: ") - 1;

        DEBUG("spic: [%*s]", len, str);
        if (sscanf(str, "%d,%d,%d,%d", &pin1, &pin2, &puk1, &puk2) == 4) {
            DEBUG("pin1: %d, pin2: %d, puk1: %d, puk2: %d", pin1, pin2, puk1, puk2);
            _inputPinTimesRemained.pin1 = pin1;
            _inputPinTimesRemained.pin2 = pin2;
            _inputPinTimesRemained.puk1 = puk1;
            _inputPinTimesRemained.puk2 = puk2;
        }
    }
}

State GsmModemPinManagement::START_CHECK_REGISTRATION(void) {
    if (isStateUpdated()) {
        DEBUG("start check registration");
    }
    if (_drv->isReadyForCmd()) {
        _nextRegCheck = millis() + _checkRegInterval;
        _drv->execCmd((GM_PCHAR *) CREG, "?");
        _drv->waitResponse();
        SM_GOTO(CHECK_REGISTRATION);
    }
}

State GsmModemPinManagement::CHECK_REGISTRATION(void) {
    if (isStateUpdated()) {
        DEBUG("check registration");
    }
    if (_drv->isCmdComplete()) {
        if (_drv->isCmdSuccess()) {
            setRegStatus(_parseRegistrationResponse(_drv->cmdTextResponse(), _drv->cmdTextResponseLen() - 2));
            //DEBUG("Response: %#*s status: %d", _drv->cmd()->textResponseLen(), _drv->cmd()->textResponse(), _regStatus);
            _drv->endCmd();

            if (_regStatus == GM_REG_REGISTERED || _regStatus == GM_REG_ROAMING) {
                DEBUG("registered");
            }
            else {
                _initializedAfterReg = false;
            }
            SM_GOTO(IDLE);
        }
        else {
            //DEBUG("Command is failed");
            setRegStatus(GM_REG_NONE);
            _initializedAfterReg = false;
            _drv->endCmd();
            SM_GOTO(IDLE);
        }
    }
}

State GsmModemPinManagement::START_INIT(void) {
    if (_initializedAfterReg) {
        SM_GOTO(IDLE);
    }
    if (_drv->isReadyForCmd()) {
        //DEBUG("send init cmd");
        _drv->execCmd(_initAfterRegCmd);
        _drv->waitResponse();
        SM_CALL2(WAIT_FOR_INIT, IDLE, IDLE);
    }
}

State GsmModemPinManagement::WAIT_FOR_INIT(void) {
    if (isStateUpdated()) {
        DEBUG("check for initialized");
    }
    if (_drv->isCmdComplete()) {
        if (_drv->isCmdSuccess()) {
            //DEBUG("initialized successfully");
            _drv->endCmd();
            _initializedAfterReg = true;
            SM_RETURN_SUCCESS();
        }
        else {
            _drv->endCmd();
            SM_RETURN_FAIL();
        }
    }
}

uint8_t GsmModemPinManagement::_parseRegistrationResponse(char *str, size_t len) {
    int n, s;

    if (strStartWith_P(str, len, CREG)) {
        str += sizeof("+CREG: ") - 1;
        len -= sizeof("+CREG: ") - 1;

        //DEBUG("creg: [%*s]", len, str);
        if (sscanf(str, "%d,%d", &n, &s) == 2) {
            //DEBUG("registration status: %d", s);
            return s;
        }
    }

    return GM_REG_NONE;;
}

uint8_t GsmModemPinManagement::_parseUrcRegistrationResponse(char *str, size_t len) {
    int s;

    if (strStartWith_P(str, len, CREG)) {
        str += sizeof("+CREG: ") - 1;
        len -= sizeof("+CREG: ") - 1;

        //DEBUG("creg: [%*s]", len, str);
        if (sscanf(str, "%d", &s) == 1) {
            //DEBUG("registration status: %d", s);
            return s;
        }
    }

    return GM_REG_NONE;
}

void GsmModemPinManagement::pinUrcDetected(GsmModemDriver *drv, char *line, size_t len, void *data) {
    GsmModemPinManagement *self = (GsmModemPinManagement *) data;

    //DEBUG("line: %s", line);
    self->setPinStatus(self->_parseInputPinResponse(line, len));
    if (self->_pinStatus == GM_PIN_READY) {
        self->_nextPinCheck = 0;
        self->_nextRegCheck = 0;
    }
    else {
        self->setRegStatus(GM_REG_NONE);
        self->_initializedAfterReg = false;
    }
}

void GsmModemPinManagement::regUrcDetected(GsmModemDriver *drv, char *line, size_t len, void *data) {
    GsmModemPinManagement *self = (GsmModemPinManagement *) data;

    //DEBUG("line: %s", line);
    self->setRegStatus(self->_parseUrcRegistrationResponse(line, len));
    if (self->_regStatus == GM_REG_REGISTERED || self->_regStatus == GM_REG_ROAMING) {
        self->_nextRegCheck = millis() + self->_checkRegInterval;
    }
    else {
        self->_initializedAfterReg = false;
    }
}

void GsmModemPinManagement::callReadyUrcDetected(GsmModemDriver *drv, char *line, size_t len, void *data) {
    GsmModemPinManagement *self = (GsmModemPinManagement *) data;

    DEBUG("line: %s", line);
    self->_nextRegCheck = 0;
}

void GsmModemPinManagement::pause(void) {
    if (isPaused())
        return;

    setPinStatus(GM_PIN_UNKNOWN);
    setRegStatus(GM_REG_NONE);
    _initializedAfterReg = false;

    StateMachine::pause();
}

void GsmModemPinManagement::restart(void) {
    if (!isPaused())
        return;

    setPinStatus(GM_PIN_UNKNOWN);
    setRegStatus(GM_REG_NONE);
    _initializedAfterReg = false;
    _nextPinCheck        = 0;
    _nextRegCheck        = 0;

    StateMachine::restart();

    SM_GOTO(IDLE);
}

void GsmModemPinManagement::begin(void) {
    _drv->addUrcHandler(CPIN, GM_MATCH_STARTS_WITH, pinUrcDetected, this);
    _drv->addUrcHandler(CREG, GM_MATCH_STARTS_WITH, regUrcDetected, this);
    _drv->addUrcHandler(CALL_READY, GM_MATCH_EQUALS, callReadyUrcDetected, this);
    pause();
}

void GsmModemPinManagement::onChangePinStatus(GsmModemPinCallback callback, void *data) {
    GsmModemPinHandler *h = (GsmModemPinHandler *) calloc(1, sizeof(GsmModemPinHandler));
    if (h == NULL) {
        //DEBUG("memory allocation error");
        while (true)
            ;
    }

    h->callback = callback;
    h->data     = data;

    h->next     = _changePinStatusHandler;
    _changePinStatusHandler = h;
}

void GsmModemPinManagement::onChangeRegStatus(GsmModemPinCallback callback, void *data) {
    GsmModemPinHandler *h = (GsmModemPinHandler *) calloc(1, sizeof(GsmModemPinHandler));
    if (h == NULL) {
        //DEBUG("memory allocation error");
        while (true)
            ;
    }

    h->callback = callback;
    h->data     = data;

    h->next     = _changeRegStatusHandler;
    _changeRegStatusHandler = h;
}

void GsmModemPinManagement::setPinStatus(uint8_t pinStatus) {
    _pinStatus = pinStatus;

    GsmModemPinHandler *h;
    for (h = _changePinStatusHandler; h != NULL; h = h->next) {
        h->callback(this, _pinStatus, h->data);
    }
}

void GsmModemPinManagement::setRegStatus(uint8_t regStatus) {
    _regStatus = regStatus;

    GsmModemPinHandler *h;
    for (h = _changeRegStatusHandler; h != NULL; h = h->next) {
        h->callback(this, _regStatus, h->data);
    }
}
