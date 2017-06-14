#include "GsmModemDriver.h"
#ifndef WITH_GSM_MODEM_DEBUG
#  undef WITH_DEBUG
#endif
#include <DebugUtil.h>

#undef SM_CLASS
#define SM_CLASS GsmModemDriver

State GsmModemDriver::IDLE(void) {
    if (isStateUpdated()) {
        DEBUG("idle");
        resetCmd();
    }
    if (isCmdWaitResponse()) {
        SM_GOTO(WAIT_CMD_RESPONSE);
    }
    if (isCmdComplete()) {
        SM_GOTO(WAIT_CMD_END);
    }
}

State GsmModemDriver::WAIT_CMD_RESPONSE(void) {
    if (isStateUpdated()) {
        DEBUG("wait response");
    }
    if (isCmdComplete()) {
        SM_GOTO(WAIT_CMD_END);
    }
    if (isTimeout(_cmd.responseTimeout())) {
        DEBUG("got timeout");
        flush();
        _cmd.completeWithTimeout();
        SM_GOTO(WAIT_CMD_END);
    }
}

State GsmModemDriver::WAIT_CMD_END(void) {
    if (isStateUpdated()) {
        DEBUG("cmd complete");
    }
    if (isCmdEnd()) {
        SM_GOTO(IDLE);
    }
}

void GsmModemDriver::_sendCmdParamsAndExec(const char *param1, const char *param2, const char *param3) {
    if (param1 != NULL) {
        print(param1);
    }
    if (param2 != NULL) {
        print(param2);
    }
    if (param3 != NULL) {
        print(param3);
    }
    print(GM_CR);
}

void GsmModemDriver::execCmd(const char cmd, const char *param1, const char *param2, const char *param3) {
    DEBUG("cmd: AT%c", cmd);
    print((GM_PCHAR *) AT);
    print(cmd);
    _sendCmdParamsAndExec(param1, param2, param3);
}

void GsmModemDriver::execCmd(const char *cmd, const char *param1, const char *param2, const char *param3) {
    DEBUG("cmd: AT%s", cmd);
    print((GM_PCHAR *) AT);
    if (cmd != NULL)
        print(cmd);
    _sendCmdParamsAndExec(param1, param2, param3);
}

void GsmModemDriver::execCmd(GM_PCHAR *cmd, const char *param1, const char *param2, const char *param3) {
    DEBUG("cmd: AT%P", cmd);
    print((GM_PCHAR *) AT);
    if (cmd != NULL)
        print(cmd);
    _sendCmdParamsAndExec(param1, param2, param3);
}

void GsmModemDriver::waitResponse(GM_CHAR_ARRAY *expect, uint32_t timeout) {
    if (!timeout)
        timeout = _respTimeout;
    DEBUG("wait: %d", timeout);
    _cmd.waitResponse(expect, timeout);
}

void GsmModemDriver::loop(void) {
    _read();
    StateMachine::loop();
}

void GsmModemDriver::flush(void) {
    _buf.reset();
    while (_port->read() != -1)
        ;
    //_port->flush();
}

size_t GsmModemDriver::write(uint8_t c) {
    return _port->write(c);
}

void GsmModemDriver::_onLineRead(char *line, size_t len) {
    DEBUG("read line: \"%#*s\"", len, line);
    if (isCmdWaitResponse() && strStartWith_P(line, len - 2, AT)) {
        DEBUG("detected command echo");
        _buf.reset();
        _cmd.beginResponse(_buf.start);
    }
    else if (_cmd.isBeginResponse()) {
        DEBUG("check if command is complete");
        GM_CHAR *response = strMatch_P(line, len - 2, _cmd.expect()); // 2 = CR + LF
        if (response != NULL) {
            DEBUG("complete: %P", response);
            _cmd.complete(response, line);
            _buf.removeLine(line, line + len);
        }
    }
    else {
        // check URC
        GsmModemUrcHandler *h;

        for (h = _urcHandler; h != NULL; h = h->next) {
            if (strMatch_P(line, len -2, h->pattern, h->matchingType)) {
                h->callback(this, line, len - 2, h->data);
                break;
            }
        }
        _buf.removeLine(line, line + len);
    }
}

void GsmModemDriver::_read(void) {
    int   c;
    char *line;

    if (!_buf.available() || !_port->available())
        return;

    while (_buf.available() && (c = _port->read()) != -1) {
        line = _buf.lastLine();
        _buf.put(c);

        if (c == GM_LF) {
            if ((_buf.last - line - 2) > 0) { // 2 = CR + LF
                _onLineRead(line, _buf.last - line);
            }
            else {
                _buf.removeLine(line, _buf.last);
            }
        }
        else if (strcmp(line, GM_INPUT_PROMPT) == 0) {
            DEBUG("Wait for input");
            _cmd.waitForInput();
        }
    }
}

void GsmModemDriver::addUrcHandler(GM_CHAR *pattern, uint8_t matchingType,
                                   GsmModemUrcCallback callback, void *data)
{
    GsmModemUrcHandler *h = (GsmModemUrcHandler *) calloc(1, sizeof(GsmModemUrcHandler));
    if (h == NULL) {
        //DEBUG("memory allocation error");
        while (true)
            ;
    }

    h->pattern      = pattern;
    h->matchingType = matchingType,
    h->callback     = callback;
    h->data         = data;

    h->next     = _urcHandler;
    _urcHandler = h;
}

void GsmModemDriver::begin(void) {
    _buf.begin();
    _cmd.begin();
}
