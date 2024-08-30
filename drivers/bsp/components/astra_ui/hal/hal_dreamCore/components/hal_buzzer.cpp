//
// Created by Fir on 2024/2/11.
//
#include "../hal_dreamCore.h"

void HALDreamCore::_beepStart()
{
    key_value_msg("Beep_Start", NULL, 0);
}

void HALDreamCore::_beepStop()
{
    key_value_msg("Beep_Stop", NULL, 0);
}
