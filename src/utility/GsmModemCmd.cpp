#include "GsmModemCmd.h"
#ifndef WITH_GSM_MODEM_DEBUG
#  undef WITH_DEBUG
#endif
#include <DebugUtil.h>

void GsmModemCmd::begin(void) {
    reset();
}

void GsmModemCmd::reset(void) {
    _status          = GM_CMD_READY;
    _expect          = NULL;
    _response        = NULL;
    _textResponse    = NULL;
    _textResponseLen = 0;
    _isWaitForInput  = false;
}

void GsmModemCmd::beginResponse(char *response) {
    _textResponse = response;
    _status       = GM_CMD_BEGIN_RESPONSE;
}

void GsmModemCmd::waitResponse(const char* const  *expect, uint32_t timeout) {
    _status          = GM_CMD_WAIT_RESPONSE;
    _expect          = expect;
    _responseTimeout = timeout;
}

void GsmModemCmd::complete(PGM_P response, char *endResponse) {
    _status          = GM_CMD_COMPLETE;
    _response        = response;

    if (endResponse != NULL)
        _textResponseLen = endResponse - _textResponse;
}
