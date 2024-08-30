//
// Created by Fir on 2024/2/11.
//

#pragma once
#ifndef ASTRA_CORE_SRC_HAL_HAL_DREAMCORE_HAL_DREAMCORE_H_
#define ASTRA_CORE_SRC_HAL_HAL_DREAMCORE_HAL_DREAMCORE_H_
#include "../hal.h"
#include "u8g2.h"

class HALDreamCore : public HAL
{
private:
  void _RTOS_Init();
  void _u8g2_init();

public:
  HALDreamCore() = default;

protected:
  // 当你声明 u8g2_t canvasBuffer {}; 时，程序会在内存中分配一块空间来存储这个结构体
  // 这个结构体中包含了指向实际图形数据的指针，以及控制显示的各种参数。
  // 虽然 canvasBuffer 看起来只是一个变量，但它实际上代表了一个完整的图形缓冲系统。
  // 当你调用绘图函数时，这些函数会修改 canvasBuffer 所管理的内存区域。
  // 这个内存区域就是我们比喻中的"画布"，每个像素对应内存中的一个或多个位。
  u8g2_t canvasBuffer{};
  static unsigned char _u8x8_byte_hw_spi_callback(u8x8_t *_u8x8, unsigned char _msg, unsigned char _argInt, void *_argPtr);

  static unsigned char _u8x8_gpio_and_delay_callback(__attribute__((unused)) u8x8_t *_u8x8,
                                                     __attribute__((unused)) unsigned char _msg,
                                                     __attribute__((unused)) unsigned char _argInt,
                                                     __attribute__((unused)) void *_argPtr);

public:
  inline void init() override
  {
    _RTOS_Init();
    _u8g2_init();
  }

protected:
  static void _st7567_reset(bool _state);

public:
  void _screenOn() override;
  void _screenOff() override;

public:
  void *_getCanvasBuffer() override;
  unsigned char _getBufferTileHeight() override;
  unsigned char _getBufferTileWidth() override;
  void _canvasUpdate() override;
  void _canvasClear() override;
  void _setFont(const unsigned char *_font) override;
  unsigned char _getFontWidth(std::string &_text) override;
  unsigned char _getFontHeight() override;
  void _setDrawType(unsigned char _type) override;
  void _drawPixel(float _x, float _y) override;
  void _drawEnglish(float _x, float _y, const std::string &_text) override;
  void _drawEnglish_str(float _x, float _y, const char *str) override;
  void _drawChinese(float _x, float _y, const std::string &_text) override;
  void _drawVDottedLine(float _x, float _y, float _h) override;
  void _drawHDottedLine(float _x, float _y, float _l) override;
  void _drawVLine(float _x, float _y, float _h) override;
  void _drawHLine(float _x, float _y, float _l) override;
  void _drawBMP(float _x, float _y, float _w, float _h, const unsigned char *_bitMap) override;
  void _drawBox(float _x, float _y, float _w, float _h) override;
  void _drawRBox(float _x, float _y, float _w, float _h, float _r) override;
  void _drawFrame(float _x, float _y, float _w, float _h) override;
  void _drawRFrame(float _x, float _y, float _w, float _h, float _r) override;

public:
  void _delay(unsigned long _mill) override;
  unsigned long _millis() override;
  unsigned long _getTick() override;
  unsigned long _getRandomSeed() override;

public:
  void _beepStart() override;
  void _beepStop() override;

public:
  void _updateConfig() override;
};

#endif // ASTRA_CORE_SRC_HAL_HAL_DREAMCORE_HAL_DREAMCORE_H_
