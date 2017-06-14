#ifdef WITH_GSM_MODEM_INFO

#ifndef WITH_GSM_MODEM_DEBUG
#  undef WITH_DEBUG
#endif
#include <DebugUtil.h>
#include <StringUtil.h>
#include "GsmModemInfo.h"
#include <stdlib.h>

#undef SM_CLASS
#define SM_CLASS GsmModemInfo

#define _GPRS_ATTACH_TIMEOUT  20000
#define _GET_LOCATION_TIMEOUT 20000
#define _RETRY_FAILED_TIMEOUT 10000

// Command
GM_PROG_CHAR GsmModemInfo::CGMM[]      = "+CGMM";
GM_PROG_CHAR GsmModemInfo::CGMR[]      = "+CGMR";
GM_PROG_CHAR GsmModemInfo::CGSN[]      = "+CGSN";
GM_PROG_CHAR GsmModemInfo::CSQ[]       = "+CSQ";
GM_PROG_CHAR GsmModemInfo::CBC[]       = "+CBC";
GM_PROG_CHAR GsmModemInfo::CMTE[]      = "+CMTE";
GM_PROG_CHAR GsmModemInfo::CGATT[]     = "+CGATT";
GM_PROG_CHAR GsmModemInfo::SAPBR[]     = "+SAPBR";
GM_PROG_CHAR GsmModemInfo::CIPGSMLOC[] = "+CIPGSMLOC";

State GsmModemInfo::IDLE(void) {
    if (isStateUpdated()) {
        DEBUG("idle");
    }
    if ((_model == NULL || _revision == NULL || _imei == NULL) && millis() > _nextUpdateModel
        && (_pm->getStatus() & GM_PM_INITIALIZED)) {
        SM_GOTO(GET_MODEL);
    }
    if (millis() > _nextUpdateBatteryCharge
        && (_pm->getStatus() & GM_PM_INITIALIZED)) {
        SM_GOTO(GET_BATTERY_CHARGE);
    }
    if (millis() > _nextUpdateSignalQuality
        && (_pin->getRegStatus() & (GM_REG_REGISTERED | GM_REG_ROAMING))) {
        SM_GOTO(GET_SIGNAL_QUALITY);
    }
    if (millis() > _nextUpdateLocation
        && (_pin->getRegStatus() & (GM_REG_REGISTERED | GM_REG_ROAMING))) {
        SM_CALL2(GPRS_ATTACH, GET_LOCATION, GET_LOCATION_FAIL);
    }
}

State GsmModemInfo::PARSE_RESPONSE(void) {
    if (isStateUpdated()) {
        DEBUG("parse response");
    }
    if (_drv->isCmdComplete()) {
        if (_drv->isCmdSuccess()) {
            DEBUG("Response: %#*s", _drv->cmdTextResponseLen(), _drv->cmdTextResponse());
            _parser(this, _drv->cmdTextResponse(), _drv->cmdTextResponseLen() - 2);
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

State GsmModemInfo::GET_MODEL(void) {
    if (isStateUpdated()) {
        DEBUG("get model");
    }
    if (_drv->isReadyForCmd()) {
        _nextUpdateModel = millis() + _updateModelInterval;
        _drv->execCmd((GM_PCHAR *) CGMM);
        _drv->waitResponse();
        _parser       = _parseSimpleResponse;
        _parserResult = &_model;
        SM_CALL2(PARSE_RESPONSE, GET_REVISION, GET_REVISION);
    }
}

State GsmModemInfo::GET_REVISION(void) {
    if (isStateUpdated()) {
        DEBUG("get revision");
    }
    if (_drv->isReadyForCmd()) {
        _drv->execCmd((GM_PCHAR *) CGMR);
        _drv->waitResponse();
        _parser       = _parseSimpleResponse;
        _parserResult = &_revision;
        SM_CALL2(PARSE_RESPONSE, GET_IMEI, GET_IMEI);
    }
}

State GsmModemInfo::GET_IMEI(void) {
    if (isStateUpdated()) {
        DEBUG("get IMEI");
    }
    if (_drv->isReadyForCmd()) {
        _drv->execCmd((GM_PCHAR *) CGSN);
        _drv->waitResponse();
        _parser       = _parseSimpleResponse;
        _parserResult = &_imei;
        SM_CALL2(PARSE_RESPONSE, IDLE, IDLE);
    }
}

State GsmModemInfo::GET_SIGNAL_QUALITY(void) {
    if (isStateUpdated()) {
        DEBUG("get signal quality");
    }
    if (_drv->isReadyForCmd()) {
        _nextUpdateSignalQuality = millis() + _updateSignalQualityInterval;
        _drv->execCmd((GM_PCHAR *) CSQ);
        _drv->waitResponse();
        _parser = _parseSignalQuality;
        SM_CALL2(PARSE_RESPONSE, IDLE, IDLE);
    }
}

State GsmModemInfo::GET_BATTERY_CHARGE(void) {
    if (isStateUpdated()) {
        DEBUG("get battery charge");
    }
    if (_drv->isReadyForCmd()) {
        _nextUpdateBatteryCharge = millis() + _updateBatteryChargeInterval;
        _drv->execCmd((GM_PCHAR *) CBC);
        _drv->waitResponse();
        _parser = _parseBatteryCharge;
        SM_CALL2(PARSE_RESPONSE, GET_TEMPERATURE, GET_TEMPERATURE);
    }
}

State GsmModemInfo::GET_TEMPERATURE(void) {
    if (isStateUpdated()) {
        DEBUG("get temperature");
    }
    if (_drv->isReadyForCmd()) {
        _drv->execCmd((GM_PCHAR *) CMTE, "?");
        _drv->waitResponse();
        _parser = _parseTemperature;
        SM_CALL2(PARSE_RESPONSE, IDLE, IDLE);
    }
}

State GsmModemInfo::GET_LOCATION(void) {
    if (isStateUpdated()) {
        DEBUG("get location");
    }
    if (_drv->isReadyForCmd()) {
        _drv->execCmd((GM_PCHAR *) CIPGSMLOC, "=1,1");
        _drv->waitResponse(STD_RESP, _GET_LOCATION_TIMEOUT);
        _parser = _parseLocation;
        SM_CALL2(PARSE_RESPONSE, GET_LOCATION_SUCCESS, GET_LOCATION_FAIL);
    }
}

State GsmModemInfo::GET_LOCATION_SUCCESS(void) {
    _nextUpdateLocation = millis() + _updateLocationInterval;
    SM_CALL2(GPRS_DETACH, IDLE, IDLE);
}

State GsmModemInfo::GET_LOCATION_FAIL(void) {
    _nextUpdateLocation = millis() + _RETRY_FAILED_TIMEOUT;
    SM_CALL2(GPRS_DETACH, IDLE, IDLE);
}

void GsmModemInfo::_parseLocation(GsmModemInfo *self, char *response, size_t len) {
    if (strStartWith_P(response, len, CIPGSMLOC)) {
        response += sizeof("+CIPGSMLOC: ") - 1;
        len      -= sizeof("+CIPGSMLOC: ") - 1;

        char  *endptr;
        float lon, lat;
        strtol(response, &endptr, 10);
        if (endptr != NULL && *endptr == ',') {
            response = endptr + 1;
            lon = strtod(response, &endptr);
            if (endptr != NULL && *endptr == ',') {
                response = endptr + 1;
                lat = strtod(response, &endptr);
                if (endptr != NULL && *endptr == ',') {
                    self->_longitude = lon;
                    self->_latitude  = lat;
                    return;
                }
            }
        }
    }
}

void GsmModemInfo::_parseSignalQuality(GsmModemInfo *self, char *response, size_t len) {
    if (strStartWith_P(response, len, CSQ)) {
        response += sizeof("+CSQ: ") - 1;
        len      -= sizeof("+CSQ: ") - 1;

        int rssi, ber;
        if (sscanf(response, "%d,%d", &rssi, &ber) == 2) {
            self->_signalQuality = rssi;
            return;
        }
    }

    self->_signalQuality = 0;
}

void GsmModemInfo::_parseBatteryCharge(GsmModemInfo *self, char *response, size_t len) {
    if (strStartWith_P(response, len, CBC)) {
        response += sizeof("+CBC: ") - 1;
        len      -= sizeof("+CBC: ") - 1;

        int status, level, voltage;
        if (sscanf(response, "%d,%d,%d", &status, &level, &voltage) == 3) {
            self->_batteryLevel   = level;
            self->_batteryVoltage = voltage;
        }
    }
}

void GsmModemInfo::_parseTemperature(GsmModemInfo *self, char *response, size_t len) {
    if (strStartWith_P(response, len, CMTE)) {
        response += sizeof("+CMTE: ") - 1;
        len      -= sizeof("+CMTE: ") - 1;

        int mode, temp;
        if (sscanf(response, "%d,%d", &mode, &temp) == 2) {
            self->_temperature = temp;
            return;
        }
    }

    self->_temperature = 0;
}

void GsmModemInfo::_parseSimpleResponse(GsmModemInfo *self, char *response, size_t len) {
    *self->_parserResult = (char *) malloc(len + 1);
    memcpy(*self->_parserResult, response, len);
    (*self->_parserResult)[len] = '\0';
}

void GsmModemInfo::begin(void) {
}

void GsmModemInfo::printJson(Print &out) {
    uint8_t powerStatus = _pm->getStatus();

    StringUtil::fprintf(out, F("{\"model\":\"%s\","
        "\"revision\":\"%s\","
        "\"IMEI\":\"%s\","
        "\"signalQuality\":%d,"
        "\"temperature\":%d,"
        "\"battery\":{\"level\":%d,\"voltage\":%d},"
        "\"location\":{\"longitude\":%f,\"latitude\":%f},"
        "\"power\":"),
        _model    == NULL ? "UNKNOWN" : _model,
        _revision == NULL ? "UNKNOWN" : _revision,
        _imei     == NULL ? "UNKNOWN" : _imei,
        _signalQuality,
        _temperature,
        _batteryLevel,
        _batteryVoltage,
        _longitude,
        _latitude);

    printPowerStatus(out, powerStatus);
    if ((powerStatus & (GM_PM_PWR_STATE | GM_PM_INITIALIZED)) == (GM_PM_PWR_STATE | GM_PM_INITIALIZED)) {
        out.print(F(",\"pin\":"));
        printPinStatus(out);

        out.print(F(",\"vcs\":"));
        printVcsStatus(out);
    }
    out.print("}");
}

void GsmModemInfo::printPowerStatus(Print &out, uint8_t powerStatus) {
    StringUtil::fprintf(out, F("{\"on\":%d,\"ready\":%d,\"initialized\":%d}"),
        powerStatus & GM_PM_PWR_STATE   ? 1 : 0,
        powerStatus & GM_PM_READY       ? 1 : 0,
        powerStatus & GM_PM_INITIALIZED ? 1 : 0);
}

void GsmModemInfo::printPinStatus(Print &out) {
    uint8_t pinStatus = _pin->getPinStatus();
    uint8_t regStatus = _pin->getRegStatus();

    out.print(F("{\"status\":\""));
    if (pinStatus == GM_PIN_READY) {
        out.print(F("READY"));
    }
    else if (pinStatus == GM_PIN_NOT_READY) {
        out.print(F("NOT_READY"));
    }
    else if (pinStatus == GM_PIN_NOT_INSERTED) {
        out.print(F("NOT_INSERTED"));
    }
    else if (pinStatus == GM_PIN_PH_SIM_PIN) {
        out.print(F("PH_SIM_PIN"));
    }
    else if (pinStatus == GM_PIN_PH_SIM_PUK) {
        out.print(F("PH_SIM_PUK"));
    }
    else if (pinStatus == GM_PIN_SIM_PIN) {
        out.print(F("SIM_PIN"));
    }
    else if (pinStatus == GM_PIN_SIM_PUK) {
        out.print(F("SIM_PUK"));
    }
    else if (pinStatus == GM_PIN_SIM_PIN2) {
        out.print(F("SIM_PIN2"));
    }
    else if (pinStatus == GM_PIN_SIM_PUK2) {
        out.print(F("SIM_PUK2"));
    }
    else {
        out.print(F("UNKNOWN"));
    }

    GsmModemInputPinTimesRemained *timesRemained = _pin->getInputPinTimesRemained();
    StringUtil::fprintf(out, F("\",\"timesRemained\":{\"pin1\":%d,\"pin2\":%d,\"puk1\":%d,\"puk2\":%d}"),
        timesRemained->pin1,
        timesRemained->pin2,
        timesRemained->puk1,
        timesRemained->puk2);

    if (pinStatus == GM_PIN_READY) {
        out.print(F(",\"reg\":\""));
        if (regStatus == GM_REG_REGISTERED) {
            out.print(F("REGISTERED"));
        }
        else if (regStatus == GM_REG_SEARCHING) {
            out.print(F("SEARCHING"));
        }
        else if (regStatus == GM_REG_DENIED) {
            out.print(F("DENIED"));
        }
        else if (regStatus == GM_REG_ROAMING) {
            out.print(F("ROAMING"));
        }
        else {
            out.print(F("UNKNOWN"));
        }
        out.print("\"");
    }
    out.print("}");
}

void GsmModemInfo::printVcsStatus(Print &out) {
    uint8_t status = _vcs->getStatus();

    out.print(F("{\"status\":\""));
    if (status == GM_VCS_CALLING) {
        out.print(F("CALLING"));
    }
    else if (status == GM_VCS_RECEIVING_CALL) {
        out.print(F("RECEIVING_CALL"));
    }
    else if (status == GM_VCS_TALKING) {
        out.print(F("TALKING"));
    }
    else if (status == GM_VCS_HANGING_UP) {
        out.print(F("HANGING_UP"));
    }
    else if (status == GM_VCS_ANSWERING) {
        out.print(F("ANSWERING"));
    }
    else {
        out.print(F("IDLE"));
    }

    StringUtil::fprintf(out, F("\",\"incomingCallNumber\":\"%s\","
        "\"outgoingCallNumber\":\"%s\",\"lastCallResult\":\""),
        _vcs->incomingCallNumber(),
        _vcs->outgoingCallNumber());

    uint8_t result = _vcs->lastCallResult();
    if (result == GM_VCS_RES_SUCCESS) {
        out.print(F("SUCCESS"));
    }
    else if (result == GM_VCS_RES_ERROR) {
        out.print(F("ERROR"));
    }
    else if (result == GM_VCS_RES_TIMEOUT) {
        out.print(F("TIMEOUT"));
    }
    else {
        out.print(F("NONE"));
    }

    out.print(F("\"}"));
}

#endif /* WITH_GSM_MODEM_INFO*/
