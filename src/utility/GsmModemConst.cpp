#include "GsmModemConst.h"
#ifndef WITH_GSM_MODEM_DEBUG
#  undef WITH_DEBUG
#endif
#include <DebugUtil.h>

GM_PROG_CHAR GsmModemConst::AT[]               = "AT";
GM_PROG_CHAR GsmModemConst::ATTENTION[]        = "+++";

// Standart responses
GM_PROG_CHAR GsmModemConst::OK[]               = "OK";
GM_PROG_CHAR GsmModemConst::SHUT_OK[]          = "SHUT OK";
GM_PROG_CHAR GsmModemConst::ERROR[]            = "ERROR";
GM_PROG_CHAR_ARRAY GsmModemConst::STD_RESP[]   = {GsmModemConst::OK,
                                                  GsmModemConst::SHUT_OK,
                                                  GsmModemConst::ERROR,
                                                  NULL};
