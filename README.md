# Fully asynchronous GSM library [![Build Status](https://travis-ci.org/yoreek/Arduino-GsmModem.svg?branch=master)](https://travis-ci.org/yoreek/Arduino-GsmModem)

* Version: 1.0.0
* Release Date: 2017-06-14

## How do I get set up? ##

 * Download and Install [Time](https://github.com/yoreek/Time) library.
 * Download and Install [DebugUtil](https://github.com/yoreek/Arduino-DebugUtil) library.
 * Download and Install [StringUtil](https://github.com/yoreek/Arduino-StringUtil) library.
 * Download and Install [StateMachine](https://github.com/yoreek/Arduino-StateMachine) library.
 * [Download](https://github.com/yoreek/Arduino-GsmModem/archive/master.zip) the Latest release from gitHub.
 * Unzip and modify the Folder name to "GsmModem".
 * Paste the modified folder on your library folder (On your `Libraries` folder inside Sketchbooks or Arduino software).
 * Restart the Arduino Software


## Usage ##

```
#include <Arduino.h>
#include <DebugUtil.h>
#include <SIM900.h>

#define gsmSerial Serial3

#define GSM_MODEM_BUF_SIZE  127
#define GSM_MODEM_POWER_PIN 9
#define GSM_MODEM_BAUDRATE  57600
#define GSM_MODEM_SIM_PIN   "0000"
#define GSM_MODEM_APN       "internet"

SIM900 gsm(&gsmSerial,
             GSM_MODEM_BUF_SIZE,
             GSM_MODEM_POWER_PIN,
             GSM_MODEM_SIM_PIN,
             GSM_MODEM_APN);

void onVoiceCall(GsmModemVoiceCallService *vcs, uint8_t status, const char *phoneNumber, const char *response, void *data) {
    if (status == GM_VCS_RECEIVING_CALL) {
        DEBUG("call from %s", phoneNumber);
        vcs->hangup();
    }
}

void setup() {
    DEBUG_SERIAL_SETUP(57600);

    gsmSerial.begin(GSM_MODEM_BAUDRATE);
    gsmSerial.flush();

    gsm.vcs()->onVoiceCall(onVoiceCall);
    gsm.begin();
}

void loop() {
    gsm.loop();
}

```

## Example ##

Included on example folder, available on Arduino IDE.


## Version History ##

 * 1.0.0 (2017-06-14): Initial version.


## Who do I talk to? ##

 * [Yoreek](https://github.com/yoreek)
