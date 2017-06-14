#include "GsmModemPowerManagement.h"
#ifndef WITH_GSM_MODEM_DEBUG
#  undef WITH_DEBUG
#endif
#include <DebugUtil.h>

#undef SM_CLASS
#define SM_CLASS GsmModemPowerManagement

State GsmModemPowerManagement::IDLE(void) {
    if (isStateUpdated()) {
        DEBUG("idle");
    }
    if (millis() > _nextCheck) {
        //DEBUG("start check for alive");
        SM_CALL2(START_CHECK_FOR_ALIVE, START_INIT, TOGGLE_POWER);
    }
}

State GsmModemPowerManagement::START_CHECK_FOR_ALIVE(void) {
    if (_drv->isReadyForCmd()) {
        //DEBUG("send command");
        _nextCheck = millis() + _checkAliveInterval;
        _drv->execCmd(GM_NOP);
        _drv->waitResponse();
        SM_GOTO(CHECK_FOR_ALIVE);
    }
}

State GsmModemPowerManagement::CHECK_FOR_ALIVE(void) {
    if (isStateUpdated()) {
        DEBUG("check for alive");
    }
    if (!_drv->isCmdComplete())
        return;

    if (_drv->isCmdSuccess()) {
        //DEBUG("success");
        _drv->endCmd();
        setStatus(getStatus() | GM_PM_PWR_STATE | GM_PM_READY );
        SM_RETURN_SUCCESS();
    }
    else {
        //DEBUG("error");
        _drv->endCmd();
        setStatus(GM_PM_NONE);
        SM_RETURN_FAIL();
    }
}

State GsmModemPowerManagement::START_INIT(void) {
    if (_status & GM_PM_INITIALIZED) {
        SM_GOTO(IDLE);
    }
    if (_drv->isReadyForCmd()) {
        _drv->execCmd(_initCmd);
        _drv->waitResponse();
        SM_CALL2(WAIT_FOR_INIT, IDLE, TOGGLE_POWER);
    }
}

State GsmModemPowerManagement::WAIT_FOR_INIT(void) {
    if (isStateUpdated()) {
        DEBUG("check for initialized");
    }
    if (!_drv->isCmdComplete())
        return;

    if (_drv->isCmdSuccess()) {
        _drv->endCmd();
        setStatus(getStatus() | GM_PM_INITIALIZED);
        SM_RETURN_SUCCESS();
    }
    else {
        _drv->endCmd();
        setStatus(GM_PM_NONE);
        SM_RETURN_FAIL();
    }
}

State GsmModemPowerManagement::TOGGLE_POWER(void) {
    DEBUG("powerPin: HIGH");
    setStatus(GM_PM_NONE);
    _powerSwitch.turnOn();
    SM_TIMED_GOTO(TOGGLE_POWER2, _powerPulseWidth);
}

State GsmModemPowerManagement::TOGGLE_POWER2(void) {
    //DEBUG("powerPin: LOW");
    _powerSwitch.turnOff();
    SM_GOTO(WAIT_FOR_READY);
}

State GsmModemPowerManagement::WAIT_FOR_READY(void) {
    if (isStateUpdated()) {
        DEBUG("wait for ready");
    }
    if (_status & GM_PM_READY) {
        _nextCheck = 0;
        SM_GOTO(IDLE);
    }
    if (isTimeout(_readyTimeout)) {
        DEBUG("got timeout");
        SM_GOTO(TOGGLE_POWER);
    }
}

void GsmModemPowerManagement::readyUrcDetected(GsmModemDriver *drv, char *line, size_t len, void *data) {
    GsmModemPowerManagement *self = (GsmModemPowerManagement *) data;

    //DEBUG("line: \"%#*s\"", len, line);
    self->setStatus(GM_PM_PWR_STATE | GM_PM_READY);
    self->_nextCheck = 0;
}

void GsmModemPowerManagement::onChangeStatus(GsmModemPmCallback callback, void *data) {
    GsmModemPmHandler *h = (GsmModemPmHandler *) calloc(1, sizeof(GsmModemPmHandler));
    if (h == NULL) {
        //DEBUG("memory allocation error");
        while (true)
            ;
    }

    h->callback = callback;
    h->data     = data;

    h->next              = _changeStatusHandler;
    _changeStatusHandler = h;
}

void GsmModemPowerManagement::setStatus(uint8_t status) {
    _status = status;

    _processChangeStatusCallback();
}

void GsmModemPowerManagement::_processChangeStatusCallback(void) {
    GsmModemPmHandler *h;
    for (h = _changeStatusHandler; h != NULL; h = h->next) {
        h->callback(this, _status, h->data);
    }
}

void GsmModemPowerManagement::begin(void) {
    _powerSwitch.begin();

    _drv->addUrcHandler(_readyPattern, GM_MATCH_EQUALS, readyUrcDetected, this);
}
