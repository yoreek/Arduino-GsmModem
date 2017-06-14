#include "GsmModemBaseService.h"
#ifndef WITH_GSM_MODEM_DEBUG
#  undef WITH_DEBUG
#endif
#include <DebugUtil.h>

#undef SM_CLASS
#define SM_CLASS GsmModemBaseService

size_t GsmModemBaseService::write(uint8_t c) {
    return _drv->write(c);
}
