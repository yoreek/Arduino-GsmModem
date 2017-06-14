#ifndef WITH_GSM_MODEM_DEBUG
#  undef WITH_DEBUG
#endif
#include <DebugUtil.h>
#include <StringUtil.h>
#include "GsmModemGprs.h"

#undef SM_CLASS
#define SM_CLASS GsmModemGprs

#define _GPRS_ATTACH_TIMEOUT  20000
#define _RETRY_FAILED_TIMEOUT 10000

// Command
GM_PROG_CHAR GsmModemGprs::CGATT[]     = "+CGATT";
GM_PROG_CHAR GsmModemGprs::SAPBR[]     = "+SAPBR";

State GsmModemGprs::GPRS_CHECK(void) {
    if (isStateUpdated()) {
        DEBUG("check GPRS command");
    }
    if (_drv->isCmdComplete()) {
        if (_drv->isCmdSuccess()) {
            _drv->endCmd();
            SM_RETURN_SUCCESS();
        }
        else {
            _drv->endCmd();
            SM_RETURN_FAIL();
        }
    }
}

State GsmModemGprs::GPRS_ATTACH(void) {
    if (isStateUpdated()) {
        DEBUG("attach GPRS");
    }
    if (_drv->isReadyForCmd()) {
        _drv->execCmd((GM_PCHAR *) CGATT, "=1");
        _drv->waitResponse(STD_RESP, _GPRS_ATTACH_TIMEOUT);
        SM_CALL2(GPRS_CHECK, GPRS_SET_CONTYPE, GPRS_FAIL);
    }
}

State GsmModemGprs::GPRS_SET_CONTYPE(void) {
    if (isStateUpdated()) {
        DEBUG("set conntype");
    }
    if (_drv->isReadyForCmd()) {
        _drv->execCmd((GM_PCHAR *) SAPBR, "=3,1,\"CONTYPE\",\"GPRS\"");
        _drv->waitResponse();
        SM_CALL2(GPRS_CHECK, GPRS_SET_APN, GPRS_FAIL);
    }
}

State GsmModemGprs::GPRS_SET_APN(void) {
    if (isStateUpdated()) {
        DEBUG("set APN");
    }
    if (_drv->isReadyForCmd()) {
        _drv->execCmd((GM_PCHAR *) SAPBR, "=3,1,\"APN\",\"", _apn, "\"");
        _drv->waitResponse();
        if (_user == NULL || _user[0] == '\0') {
            SM_CALL2(GPRS_CHECK, GPRS_OPEN, GPRS_FAIL);
        }
        else {
            SM_CALL2(GPRS_CHECK, GPRS_SET_USER, GPRS_FAIL);
        }
    }
}

State GsmModemGprs::GPRS_SET_USER(void) {
    if (isStateUpdated()) {
        DEBUG("set user");
    }
    if (_drv->isReadyForCmd()) {
        _drv->execCmd((GM_PCHAR *) SAPBR, "=3,1,\"USER\",\"", _user, "\"");
        _drv->waitResponse();
        SM_CALL2(GPRS_CHECK, GPRS_SET_PWD, GPRS_FAIL);
    }
}

State GsmModemGprs::GPRS_SET_PWD(void) {
    if (isStateUpdated()) {
        DEBUG("set pwd");
    }
    if (_drv->isReadyForCmd()) {
        _drv->execCmd((GM_PCHAR *) SAPBR, "=3,1,\"PWD\",\"", _password, "\"");
        _drv->waitResponse();
        SM_CALL2(GPRS_CHECK, GPRS_OPEN, GPRS_FAIL);
    }
}

State GsmModemGprs::GPRS_OPEN(void) {
    if (isStateUpdated()) {
        DEBUG("gprs open");
    }
    if (_drv->isReadyForCmd()) {
        _drv->execCmd((GM_PCHAR *) SAPBR, "=1,1");
        _drv->waitResponse();
        SM_CALL2(GPRS_CHECK, GPRS_SUCCESS, GPRS_FAIL);
    }
}

State GsmModemGprs::GPRS_SUCCESS(void) {
    SM_RETURN_SUCCESS();
}

State GsmModemGprs::GPRS_FAIL(void) {
    SM_RETURN_FAIL();
}

State GsmModemGprs::GPRS_DETACH(void) {
    if (isStateUpdated()) {
        DEBUG("detach gprs");
    }
    if (_drv->isReadyForCmd()) {
        _drv->execCmd((GM_PCHAR *) CGATT, "=0");
        _drv->waitResponse();
        SM_CALL2(GPRS_CHECK, GPRS_SUCCESS, GPRS_FAIL);
    }
}
