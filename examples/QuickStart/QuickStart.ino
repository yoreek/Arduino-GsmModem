#if defined(ARDUINO) && ARDUINO >= 100
#  include "Arduino.h"
#else
#  include "WProgram.h"
#endif
#define WITH_DEBUG
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
