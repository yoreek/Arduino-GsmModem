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
#ifndef _GSM_MODEM_INFO_H_
#define _GSM_MODEM_INFO_H_

#ifdef WITH_GSM_MODEM_INFO
#if defined(ARDUINO) && ARDUINO >= 100
#  include "Arduino.h"
#else
#  include "WProgram.h"
#endif
#include "GsmModemPowerManagement.h"
#include "GsmModemPinManagement.h"
#include "GsmModemVoiceCallService.h"
#include "GsmModemGprs.h"

#define _UPDATE_MODEL_INTERVAL          3600000
#define _UPDATE_SIGNAL_QUALITY_INTERVAL 60000
#define _UPDATE_BATTERY_CHARGE_INTERVAL 60000
#define _UPDATE_LOCATION_INTERVAL       3600000

class GsmModemInfo;

typedef void (*GsmModemInfoParser)
(GsmModemInfo *self, char *response, size_t len);

class GsmModemInfo: public GsmModemGprs {
    public:
        GsmModemInfo(GsmModemDriver *drv,
                     GsmModemPowerManagement *pm,
                     GsmModemPinManagement *pin,
                     GsmModemVoiceCallService *vcs,
                     const char *apn,
                     const char *user     = NULL,
                     const char *password = NULL) :
            GsmModemGprs(drv, apn, user, password),
            _pm(pm),
            _pin(pin),
            _vcs(vcs),
            _model(NULL),
            _revision(NULL),
            _imei(NULL),
            _signalQuality(0),
            _batteryLevel(0),
            _batteryVoltage(0),
            _temperature(0),
            _longitude(0),
            _latitude(0),
            _nextUpdateModel(0),
            _updateModelInterval(_UPDATE_MODEL_INTERVAL),
            _nextUpdateSignalQuality(0),
            _updateSignalQualityInterval(_UPDATE_SIGNAL_QUALITY_INTERVAL),
            _nextUpdateBatteryCharge(0),
            _updateBatteryChargeInterval(_UPDATE_BATTERY_CHARGE_INTERVAL),
            _nextUpdateLocation(0),
            _updateLocationInterval(_UPDATE_LOCATION_INTERVAL)
        {
        };

        // Command
        static GM_PROG_CHAR CGMM[];
        static GM_PROG_CHAR CGMR[];
        static GM_PROG_CHAR CGSN[];
        static GM_PROG_CHAR CSQ[];
        static GM_PROG_CHAR CBC[];
        static GM_PROG_CHAR CMTE[];
        static GM_PROG_CHAR CGATT[];
        static GM_PROG_CHAR SAPBR[];
        static GM_PROG_CHAR CIPGSMLOC[];

        void begin(void);
        void printJson(Print &out);
        void printPowerStatus(Print &out, uint8_t powerStatus);
        void printPinStatus(Print &out);
        void printVcsStatus(Print &out);
        inline char *getModel(void) {
            return _model;
        };
        inline char *getRevision(void) {
            return _revision;
        };
        inline char *getIMEI(void) {
            return _imei;
        };
        inline uint8_t getSignalQuality(void) {
            return _signalQuality;
        };
        inline uint8_t getBatteryLevel(void) {
            return _batteryLevel;
        };
        inline uint8_t getBatteryVoltage(void) {
            return _batteryVoltage;
        };
        inline int8_t getTemperature(void) {
            return _temperature;
        };
        inline float getLongitude(void) {
            return _longitude;
        };
        inline float getLatitude(void) {
            return _latitude;
        };
        inline uint32_t getUpdateModelInterval(void) {
            return _updateModelInterval;
        };
        inline void setUpdateModelInterval(uint32_t t) {
            _updateModelInterval = t;
        };
        inline uint32_t getUpdateSignalQualityInterval(void) {
            return _updateSignalQualityInterval;
        };
        inline void setUpdateSignalQualityInterval(uint32_t t) {
            _updateSignalQualityInterval = t;
        };
        inline uint32_t getUpdateBatteryChargeInterval(void) {
            return _updateBatteryChargeInterval;
        };
        inline void setUpdateBatteryChargeInterval(uint32_t t) {
            _updateBatteryChargeInterval = t;
        };
        inline uint32_t getUpdateLocationInterval(void) {
            return _updateLocationInterval;
        };
        inline void setUpdateLocationInterval(uint32_t t) {
            _updateLocationInterval = t;
        };

        State IDLE(void);
        State PARSE_RESPONSE(void);
        State GET_MODEL(void);
        State GET_REVISION(void);
        State GET_IMEI(void);
        State GET_SIGNAL_QUALITY(void);
        State GET_BATTERY_CHARGE(void);
        State GET_TEMPERATURE(void);
        State GET_LOCATION(void);
        State PARSE_LOCATION(void);
        State GET_LOCATION_SUCCESS(void);
        State GET_LOCATION_FAIL(void);

    protected:
        GsmModemPowerManagement  *_pm;
        GsmModemPinManagement    *_pin;
        GsmModemVoiceCallService *_vcs;
        char                     *_model;
        char                     *_revision;
        char                     *_imei;
        uint8_t                   _signalQuality;
        uint8_t                   _batteryLevel;
        uint16_t                  _batteryVoltage;
        int8_t                    _temperature;
        float                     _longitude;
        float                     _latitude;
        uint32_t                  _nextUpdateModel;
        uint32_t                  _updateModelInterval;
        uint32_t                  _nextUpdateSignalQuality;
        uint32_t                  _updateSignalQualityInterval;
        uint32_t                  _nextUpdateBatteryCharge;
        uint32_t                  _updateBatteryChargeInterval;
        uint32_t                  _nextUpdateLocation;
        uint32_t                  _updateLocationInterval;
        GsmModemInfoParser        _parser;
        char                    **_parserResult;

        static void _parseSignalQuality(GsmModemInfo *self, char *response, size_t len);
        static void _parseBatteryCharge(GsmModemInfo *self, char *response, size_t len);
        static void _parseTemperature(GsmModemInfo *self, char *response, size_t len);
        static void _parseLocation(GsmModemInfo *self, char *response, size_t len);
        static void _parseSimpleResponse(GsmModemInfo *self, char *response, size_t len);
};

#undef _UPDATE_MODEL_INTERVAL
#undef _UPDATE_SIGNAL_QUALITY_INTERVAL
#undef _UPDATE_BATTERY_CHARGE_INTERVAL
#undef _UPDATE_LOCATION_INTERVAL
#endif /* WITH_GSM_MODEM_INFO*/

#endif
