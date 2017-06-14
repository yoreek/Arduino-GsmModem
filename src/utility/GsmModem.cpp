#include "GsmModem.h"
#ifndef WITH_GSM_MODEM_DEBUG
#  undef WITH_DEBUG
#endif
#include <DebugUtil.h>

void GsmModem::begin(void) {
    _drv.begin();
    _pm.begin();
    _pm.onChangeStatus(_onChangePmStatus, this);
    _pin.onChangeRegStatus(_onChangeRegStatus, this);
    _pin.begin();
    _vcs.begin();
    _sms.begin();
#ifdef WITH_GSM_MODEM_INFO
    _info.begin();
#endif
}

void GsmModem::loop(void) {
    _drv.loop();
    _pm.loop();
    _pin.loop();
    _vcs.loop();
    _sms.loop();
#ifdef WITH_GSM_MODEM_INFO
    _info.loop();
#endif
}

void GsmModem::_onChangePmStatus(GsmModemPowerManagement *pm, uint8_t pmStatus, void *data) {
    GsmModem *self = (GsmModem *) data;

    //DEBUG("callback status: %d", status);

    if (pmStatus & GM_PM_INITIALIZED) {
        self->_pin.restart();
        self->_sms.restart();
    }
    else {
        self->_pin.pause();
        self->_sms.pause();
    }
}

void GsmModem::_onChangeRegStatus(GsmModemPinManagement *pin, uint8_t regStatus, void *data) {
    GsmModem *self = (GsmModem *) data;

    //DEBUG("callback status: %d", status);

    if (regStatus == GM_REG_REGISTERED || regStatus == GM_REG_ROAMING) {
        self->_vcs.restart();
    }
    else {
        self->_vcs.pause();
    }
}
