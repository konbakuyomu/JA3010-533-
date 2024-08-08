//
// Created by Fir on 2024/2/11.
//
#include "../hal_dreamCore.h"
#include "variable.h"

void HALDreamCore::_key_init() {

}

bool HALDreamCore::_getKey(key::KEY_INDEX _keyIndex) {
  // if (_keyIndex == key::KEY_0) return !GPIO_ReadInputPins(KEYIN1_PORT, KEYIN1_PIN);
  // if (_keyIndex == key::KEY_1) return !GPIO_ReadInputPins(KEYIN2_PORT, KEYIN2_PIN);

  return false;
}